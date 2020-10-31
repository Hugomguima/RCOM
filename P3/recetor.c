#include "recetor.h"

extern unsigned int packetNumber;

int main(int argc, char** argv)
{
  int fd;
  off_t index = 0;
  //unsigned char message[4096], byte;

  if ( (argc < 2) || 
        ((strcmp("/dev/ttyS0", argv[1])!=0) && 
        (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

/*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
*/
  
  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd <0) {perror(argv[1]); exit(-1); }

  if(llopen(fd, RECEIVER) == ERROR){
    puts("Error on LLOPEN");
    return -3;
  }

  puts("llopen done");

  unsigned char* start = malloc(0);
  unsigned int size, sizeStart;

  
  size = llread(fd,start);
  puts("llread done");
  sizeStart = size;


  unsigned int fileSize = 0;
  unsigned int nameSize = 0;
  unsigned char *fileName  = (unsigned char *)malloc(0);

  if(checkStart(start,&fileSize,fileName,&nameSize) == ERROR){
    puts("Error on checkStart");
    return -5;
  }


  // Loop for reading all llwrites from the emissor

   // Creates null pointer to allow realloc
  unsigned char* dataPacket;
  unsigned int packetsReaded = 0;
  unsigned int messageSize;

  unsigned char* final;

  unsigned char* result = (unsigned char*)malloc(fileSize);

  while(TRUE){
    unsigned int packetSize = 0;
    unsigned char* message = malloc(0);
    messageSize = 0;
    
    puts("entededloopasdassdasdasdasdadassdadasdasdasdas");
    if((messageSize = llread(fd,message)) == ERROR){
      puts("Error on llread data packet ");
      exit(-1);
    }
    printf("message size = %d\n",messageSize);
    
    if(message[0] == CT_END){
      puts("Reached Control End Packet");
      final = (unsigned char*)malloc(messageSize);
      memcpy(final,message,messageSize);
      break;
    }
    
    packetsReaded++;
    
    printf("Received packet number: %d\n", packetsReaded);

    dataPacket = assembleDataPacket(message,messageSize,&packetSize);

    for(int i= 0; i < packetSize; i++){
      result[index + i] = dataPacket[i];
    }

    index += packetSize;

    puts("before free");
    free(dataPacket);
    puts("after free");
  }
  
  if(checkEND(start, sizeStart, final, messageSize) == 1) {
    puts("Start and End packets are different!");
    exit(-1);
  }

  printf("Received a file of size: %u\n", fileSize);

  // Creating the file to be rewritten after protocol
  createFile(result,fileSize,fileName);


  if(llclose(fd, RECEIVER) == ERROR){
    puts("Error on LLCLOSE");
    return -3;
  }

  sleep(2);

  free(fileName);
  free(result);

  close(fd);

  return 0;
}
