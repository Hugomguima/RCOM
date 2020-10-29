#include "application.h"

int packetNumber = 0; //Global variable counting thenumber of packets being sent



unsigned char* openFile(unsigned char* filename, off_t *fileSize){

    FILE * file;
    struct stat st;
    unsigned char *data;
    

    if((file = fopen(filename,"r")) == NULL ){
        perror("Cannot open file");
        exit(-1);
    }

    stat(filename,&st);
    *fileSize = st.st_size;
    printf("FileSize read with %ld \n", *fileSize);

    data = (unsigned char *)malloc(*fileSize);
    fread(data,sizeof(unsigned char),*fileSize,file);

    if(ferror(file)){
        perror("reading file error");
        exit(-2);
    }

    return data;

}

unsigned char* parseControlPacket(unsigned int state, off_t fileSize, unsigned char* filename, int sizeFilename, int *sizeControlPacket){

    *sizeControlPacket = 9 * sizeof(unsigned char) + sizeFilename;
    unsigned char* packet = (unsigned char* )malloc(*sizeControlPacket);

    if(state == CT_START){
        packet[0] = CT_START;
    }
    else{
        packet[0] = CT_END;
    }
    packet[1] = T1;
    packet[2] = L1;
    packet[3] = (fileSize >> 24) & 0XFF;
    packet[4] = (fileSize >> 16) & 0XFF; 
    packet[5] = (fileSize >> 8) & 0XFF; 
    packet[6] =  fileSize & 0XFF;
    packet[7] = T2;
    packet[8] = sizeFilename;

    for (int i=0; i < sizeFilename; i++ ){
        packet[9 + i] = filename[i];
    }

    return packet;
}

unsigned char* parseDataPacket(unsigned char *message, off_t fileSize, int *packetSize){
    
    unsigned char *packet = (unsigned char*)malloc(fileSize + 4); 

    packet[0] = CONTROL;
    packet[1] = packetNumber % 255;
    packet[2] = fileSize / 256;
    packet[3] = fileSize % 256;

    // Preencher o packet através da mensagem
    for(int i = 0; i < *packetSize;i++){
        packet[4 + i] = message[i];
    }

    *packetSize += 4;
    packetNumber++;

    return packet;
}

//unsigned char* getDataPacket(unsigned char *message, )



