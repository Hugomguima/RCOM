#include "recetor.h"

extern unsigned int packetNumber;

int main(int argc, char** argv)
{
  int fd;
  off_t index = 0;

  if ( (argc < 2) || 
        ((strcmp("/dev/ttyS0", argv[1])!=0) && 
        (strcmp("/dev/ttyS1", argv[1])!=0) )) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    return -1;
  }

/*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
*/
  
  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd <0) {
    perror(argv[1]); 
    return -2;
  }

  if(llopen(fd, RECEIVER) == ERROR){
    puts("Error on llopen");
    return -3;
  }

  unsigned char* start = malloc(0);
  unsigned int size, sizeStart;

  
  size = llread(fd,start);
  sizeStart = size;


  unsigned int fileSize = 0;
  unsigned int nameSize = 0;
  char *fileName  = (char *)malloc(0);

  if(checkStart(start,&fileSize,fileName,&nameSize) == ERROR){
    puts("Error on checkStart");
    return -4;
  }

  // Loop for reading all llwrites from the emissor
  unsigned char* dataPacket;
  unsigned int packetsRead = 0;
  unsigned int messageSize;

  unsigned char* final;

  unsigned char* result = (unsigned char*)malloc(fileSize); // Creates null pointer to allow realloc

  while(TRUE){
    unsigned int packetSize = 0;
    unsigned char* message = malloc(0);
    messageSize = 0;
    
    if((messageSize = llread(fd,message)) == ERROR){
      puts("Error on llread data packet ");
      return -5;
    }
    printf("message size = %d\n",messageSize);
    
    if(message[0] == CT_END){
      puts("Reached Control End Packet");
      final = (unsigned char*)malloc(messageSize);
      memcpy(final,message,messageSize);
      break;
    }
    
    packetsRead++;
    
    printf("Received packet number: %d\n", packetsRead);

    dataPacket = assembleDataPacket(message,messageSize,&packetSize);

    for(int i= 0; i < packetSize; i++){
      result[index + i] = dataPacket[i];
    }

    index += packetSize;

    free(dataPacket);
  }
  
  if(checkEND(start, sizeStart, final, messageSize) == 1) {
    puts("Start and End packets are different!");
    return -6;
  }

  printf("Received a file of size: %u\n", fileSize);

  // Creating the file to be rewritten after protocol
  createFile(result,fileSize,fileName);


  if(llclose(fd, RECEIVER) == ERROR){
    puts("Error on llclose");
    return -7;
  }

  sleep(1);

  free(fileName);
  free(result);

  close(fd);

  return 0;
}
