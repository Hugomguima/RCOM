/*Non-Canonical Input Processing*/
#include "emissor.h"

extern unsigned int packetNumber;

int main(int argc, char** argv)
{
  int fd;

  if ( (argc < 3) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0))) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1 \t filename.jpg \n");
    exit(1);
  }



  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd <0) {
    perror(argv[1]);
    exit(-1);
  }

  
  // Criar método  openFile para abrir o ficheiro que será recebido como argunmento da função main

  // Dealing with the SET and UA
  if(llopen(fd, TRANSMITTER) == ERROR){
    puts("Error on LLOPEN");
    return -3;
  }

  int fileNameSize = strlen(argv[2]);
  unsigned char* filename = (unsigned char*)malloc(fileNameSize);
  filename = (unsigned char*)argv[2];

  off_t *fileSize;
  int *sizeControlPacket;

  unsigned char *data = openFile(filename,fileSize);

  // Start Control packet
  unsigned char *start = parseControlPacket(CT_START,*fileSize,filename,fileNameSize,sizeControlPacket);
  if(llwrite(fd,start,*sizeControlPacket) != 0 ){
    puts("error writing start control packet");
  }
  free(start);

  // Ciclo de envio dos packets
  int packetSize = PACKETSIZE;
  off_t index = 0;

  while(index < *fileSize && packetSize != PACKETSIZE){
    unsigned char* packet = splitPacket(data,&index,&packetSize,*fileSize);

    int length = packetSize;
    
    unsigned char* message =  parseDataPacket(packet,*fileSize,&length);

    if(llwrite(fd,message,length) != 0){
      puts("error sending data packet");
      return -5;
    }

    printf("Sent packet number: %d\n", packetNumber);

    free(message);
  }


  // End Control packet
  unsigned char *end = parseControlPacket(CT_END,*fileSize,filename,fileNameSize,sizeControlPacket);
  if(llwrite(fd,end,*sizeControlPacket) != 0 ){
    puts("error writing end control packet");
  }
  free(end);


  if(llclose(fd, TRANSMITTER) == ERROR){
    puts("Error on LLCLOSE");
    return -3;
  }

  sleep(2);

  close(fd);

  free(filename);
  free(data);


  return 0;
}
