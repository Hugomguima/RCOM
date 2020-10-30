#include "recetor.h"

int main(int argc, char** argv)
{
  int fd;
  unsigned int size;
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

  unsigned char* start;
  unsigned int size;

  size = llread(fd,start);

  unsigned int *fileSize = 0;
  unsigned int *nameSize = 0;
  unsigned char *fileName  = (unsigned char *)malloc(0);

  if(checkStart(start,fileSize,fileName,nameSize) == ERROR){
    puts("Error on checkStart");
    return -5;
  }

  // Loop for reading all llwrites from the emissor

  unsigned char* message = malloc(0); // Creates null pointer to allow realloc
  unsigned char* dataPacket;

  unsigned char* result = (unsigned char*)malloc(fileSize);

  while(TRUE){
    unsigned int messageSize;
    unsigned int *packetSize;
    if(messageSize = llread(fd,message) == -1){
      puts("Error on llread data packet ");
      exit -1;
    }
    if(message[0] == CT_END){
      puts("Reached Control End Packet");
      break;
    }

    dataPacket = assembleDataPacket(message,messageSize,packetSize);

    for(int i= 0; i < *packetSize; i++){
      result[index + i] = dataPacket[i];
    }

    index += *packetSize;

  }

  // Displaying all the message after the protocol is implemented
  for(int i = 0; i < fileSize; i++){
    printf("%x",result[i]); // Não si se aqui é suposto meter em hexadecimal ou deixo estar como unsigned char :/ 
  }

  // Creating the file to be rewritten after protocol
  createFile(result,fileSize,fileName);


  if(llclose(fd, RECEIVER) == ERROR){
    puts("Error on LLCLOSE");
    return -3;
  }

  sleep(2);

  close(fd);

  return 0;
}
