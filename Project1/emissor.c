/*Non-Canonical Input Processing*/
#include "emissor.h"

extern unsigned int packetNumber;

int main(int argc, char** argv)
{
  int fd;

  if ( (argc < 3) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0))) {
    printf("Usage:\tnserial SerialPort File path\n\tex: nserial /dev/ttyS1 \t filename.jpg \n");
    exit(1);
  }



  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd < 0) {
    perror(argv[1]);
    exit(-1);
  }

  
  // Criar método  openFile para abrir o ficheiro que será recebido como argunmento da função main

  int fileNameSize = strlen(argv[2]);
  char* filename = (char*)malloc(fileNameSize);
  filename = (char*)argv[2];



  off_t fileSize = 0;
  int sizeControlPacket = 0;

  unsigned char *data = openFile(filename, &fileSize);

  // Dealing with the SET and UA
  if(llopen(fd, TRANSMITTER) == ERROR){
    puts("Error on LLOPEN");
    return -1;
  }



  puts("File opened");

  // Start Control packet
  unsigned char *start = parseControlPacket(CT_START, fileSize, filename, fileNameSize, &sizeControlPacket);

  puts("parsectpacket done");
  if(llwrite(fd, start, sizeControlPacket) != TRUE ){
    puts("Error writing start control packet");
    return -2;
  }
  free(start);

  // Ciclo de envio dos packets
  int packetSize = PACKETSIZE;
  off_t index = 0;

  while(index < fileSize && packetSize == PACKETSIZE){
    unsigned char* packet = splitPacket(data, &index, &packetSize, fileSize);

    int length = packetSize;
    
    unsigned char* message =  parseDataPacket(packet, fileSize, &length);

    if(llwrite(fd, message, length) != TRUE){
      puts("Error sending data packet");
      return -3;
    }

    printf("Sent packet number: %d\n", packetNumber);

    free(message);
  }


  // End Control packet
  printf("size control end = %d", sizeControlPacket);
  unsigned char *end = parseControlPacket(CT_END, fileSize, filename, fileNameSize, &sizeControlPacket);
  if(llwrite(fd, end, sizeControlPacket) != TRUE ){
    puts("Error writing end control packet");
    return -4;
  }
  free(end);


  if(llclose(fd, TRANSMITTER) == ERROR){
    puts("Error on LLCLOSE");
    return -5;
  }

  sleep(1);

  close(fd);

  free(data);

  return 0;
}
