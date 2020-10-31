#include "recetor.h"

int main(int argc, char** argv)
{
  int fd;
  unsigned int size;
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

  llopen(fd, RECEIVER);

  unsigned char* buffer = malloc(0);
  size = llread(fd,buffer);

  //write(fd,buffer,size); // Aqui deve escrever a mesagem do llread

  llclose(fd,RECEIVER);

  sleep(1);

  close(fd);

  return 0;
}
