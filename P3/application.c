#include "application.h"

unsigned int packetNumber = 0; //Global variable counting thenumber of packets being sent



unsigned char* openFile(unsigned char* filename, off_t *fileSize){

    FILE * file;
    struct stat st;
    unsigned char *data;
    
    if((file = fopen(filename,"r")) == NULL ){
        perror("Cannot open file");
        exit(-1);
    }


    stat(filename,&st);
    puts("stat done");
    *fileSize = st.st_size;
    puts("Hello");
    printf("FileSize read with %ld \n", *fileSize);

    data = (unsigned char *)malloc(*fileSize);

     puts("malloc worked");

    fread(data,sizeof(unsigned char),*fileSize,file);

    puts("hey 1");
    if(ferror(file)){
        perror("reading file error");
        exit(-2);
    }

    puts("hey 2");
    return data;

}

unsigned char* parseControlPacket(unsigned int state, off_t fileSize, unsigned char* filename, int sizeFilename, int *sizeControlPacket){

    puts("here");
    
    printf("size file: %d\n",sizeFilename);
    *sizeControlPacket = (9 + sizeFilename);
    printf("size control: %d\n",*sizeControlPacket);

    unsigned char* packet = (unsigned char* )malloc(*sizeControlPacket);
    puts("here2");
    

    if(state == CT_START){
        packet[0] = CT_START;
    }
    else{
        packet[0] = CT_END;
    }
    packet[1] = T1;
    packet[2] = sizeof(fileSize);

    printf("packet2 %d\n",packet[2]);

    
    for(int i = 0; i < packet[2];i++){
        packet[3+i] = (fileSize >> (i*8)) & 0xFF;
        printf("%d\n",i);
    }

    packet[3 + packet[2]] = T2;
    packet[4 + packet[2]] = sizeFilename;

    for (int i = 0; i < sizeFilename; i++ ){
        
        packet[5 + packet[2] + i] = filename[i];
        printf("filename[%d] = %c\n",i,filename[i]);
    }

    return packet;
}

unsigned char* parseDataPacket(unsigned char *message, off_t fileSize, int *length){
    
    unsigned char *packet = (unsigned char*)malloc(fileSize + 4); 

    packet[0] = CONTROL;
    packet[1] = packetNumber % 255;
    packet[2] = fileSize / 256;
    packet[3] = fileSize % 256;

    // Preencher o packet através da mensagem
    for(int i = 0; i < *length;i++){
        packet[4 + i] = message[i];
    }

    *length += 4;
    packetNumber++;

    return packet;
}

unsigned char* splitPacket(unsigned char *packet,off_t *index, int *packetSize, off_t fileSize){

    unsigned char *splitPacket;

    if(*index + *packetSize > fileSize){
        *packetSize = fileSize - *index;
    }

    splitPacket = (unsigned char*)malloc(*packetSize);

    for(int i = 0; i < *packetSize; i++){
        splitPacket[i] = packet[*index];
        (*index)++;
    }

    return splitPacket;

}

int checkStart(unsigned char* start, unsigned int *filesize,unsigned char *name, unsigned int *nameSize){

    unsigned char* packet;
    int fileSizeBytes;

    // Checking control flag
    if(start[0] != CT_START || start[1] != T1){
        puts("check start error");
        return -1;
    }

    fileSizeBytes = (int)start[2];

    // Getting fileSize
    for(int i = 0; i < fileSizeBytes; i++){
        *filesize |= (start[3 + i] << (i*8));
    }

    if(start[fileSizeBytes + 3] != T2){
        puts("check start T2 error");
        return -1;
    } 
    
    // Getting nameSize
    *nameSize = (unsigned int)start[fileSizeBytes + 4];
    printf("name size: %d\n",*nameSize);

    // Getting fileName
    name = (unsigned char *)realloc(name, *nameSize);
    for(int i = 0; i < *nameSize; i++){
        name[i] = start[fileSizeBytes + 5 + i];
        printf(" i = %d, c = %c\n",i,name[i]);
    }

    return 0;

}

int checkEND(unsigned char *start, int startSize, unsigned char *end, int endSize) {
    int j = 5;
    printf("startSize: %d\n",startSize);
    printf("endSize: %d\n",endSize);
    
    if(startSize != endSize) {
       puts("Start and End have differente sizes");
        return 1;
    }
    else {
        if(end[0] == CT_END) {
            for(int i = 5; i < startSize; i++) {
                if(start[i] != end[j]) {
                    puts("Different value");
                    return 1;
                }
                else {
                    j++;
                }  
            }
            return 0;
        }
        else {
            puts("Other reason");
            return 1;
        }
    }
}

unsigned char* assembleDataPacket(unsigned char* message, unsigned int messageSize, unsigned int *packetSize){

    *packetSize = messageSize - 4;
    unsigned char* packet = (unsigned char *)malloc(*packetSize);

    for(int i = 0; i < *packetSize;i++){
        packet[i] = message[4 + i];
    }

    return packet;
}

void createFile(unsigned char* data, unsigned int fileSize, unsigned char *filename){
    FILE *file = fopen(filename,"wb");
    fwrite(data,1,fileSize,file);
    puts("New file created!!");
    printf("FileSize written: %u\n ",fileSize);
    fclose(file);
}



