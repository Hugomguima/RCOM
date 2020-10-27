/*Non-Canonical Input Processing*/
#include "emissor.h"

int main(int argc, char** argv)
{
  int fd;

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
  if (fd <0) {
    perror(argv[1]);
    exit(-1);
  }

  unsigned char message[2] = {0x11,0x22};

  // Dealing with the SET and UA
  llopen(fd, TRANSMITTER);

  llwrite(fd, message, 2);

  llclose(fd, TRANSMITTER);

  close(fd);
  return 0;
}
