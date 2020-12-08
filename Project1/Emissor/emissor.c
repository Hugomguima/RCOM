/*Non-Canonical Input Processing*/
#include "emissor.h"

extern unsigned int packetNumber;

int main(int argc, char** argv)
{
  int fd;

  if ((argc < 3) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0))) {
    printf("Usage:\tnserial SerialPort File path\n\tex: nserial /dev/ttyS1 \t filename.jpg \n");
    return -1;
  }

  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */
  
  struct timespec initialTime, finalTime;
  clock_gettime(CLOCK_REALTIME, &initialTime);

  if ((fd = open(argv[1], O_RDWR | O_NOCTTY )) < 0) {
    perror(argv[1]);
    return -2;
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
    puts("TRANSMITTER: Error on llopen");
    return -3;
  }

  // Start Control packet
  unsigned char *start = parseControlPacket(CT_START, fileSize, filename, fileNameSize, &sizeControlPacket);

  if(llwrite(fd, start, sizeControlPacket) != TRUE ){
    puts("TRANSMITTER: Error writing START control packet");
    return -4;
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
      puts("TRANSMITTER: Error sending data packet");
      return -5;
    }

    printf("Sent packet number: %d\n", packetNumber);

    free(message);
  }


  // End Control packet
  unsigned char *end = parseControlPacket(CT_END, fileSize, filename, fileNameSize, &sizeControlPacket);

  if(llwrite(fd, end, sizeControlPacket) != TRUE ){
    puts("TRANSMITTER: Error writing END control packet");
    return -6;
  }
  free(end);


  if(llclose(fd, TRANSMITTER) == ERROR){
    puts("TRANSMITTER: Error on llclose");
    return -7;
  }

  clock_gettime(CLOCK_REALTIME, &finalTime);

  double accum = (finalTime.tv_sec - initialTime.tv_sec) + (finalTime.tv_nsec - initialTime.tv_nsec) / 1E9;

  printf("Seconds passed: %f\n", accum);

  sleep(1);
  close(fd);
  free(data);

  return 0;
}
