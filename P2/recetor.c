/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07

volatile int STOP=FALSE;

void waitSetMachine(int fd) {
  tcflush(fd, TCIOFLUSH); //limpa informacao recebida mas nao lida e informacao escrita mas nao transmitida

  enum state {START,FLAG_RCV,A_RCV,C_RCV,BCC_OK,STOP};
  enum state current = START;

  int finish = FALSE;
  unsigned char r, check;

  while(finish == FALSE) {
    read(fd, &r, 1);

    switch(current){
      case START:
        if(r == FLAG) {
          puts("Flag Received");
          current = FLAG_RCV;
        }
        break;
      case FLAG_RCV:
        if (r == A) {
          puts("A Received");
          current = A_RCV;
          check ^= r;
        }
        else if (r == FLAG);
        else {
          current = START;
        }
        break;
      case A_RCV:
        if (r == C_SET) {
          puts("C_SET Received");
          current = C_RCV;
          check ^= r;
        }
        else if (r == FLAG) {
          current = FLAG_RCV;
        }
        else {
          current = START;
        }
        break;
      case C_RCV:
        if (r == check) {
          puts("BCC OK");
          current = BCC_OK;
        }
        else if (r == FLAG) {
          current = FLAG_RCV;
        }
        else {
          current = START;
        }
        break;
      case BCC_OK:
        if (r == FLAG) {
          puts("SET correct");
          current = STOP;
        }
        else {
          current = START;
        }
        break;
      case STOP:
        break;
      default:
        break;
    }
  }
}

int main(int argc, char** argv)
{
  int fd,c;
  struct termios oldtio,newtio;
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

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

/* 
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
  leitura do(s) pr�ximo(s) caracter(es)
*/

  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("Waiting for a message...\n");

  ssize_t res;
  size_t num_bytes_read = 0;

  waitSetMachine(fd);

  tcflush(fd, TCIOFLUSH);

  unsigned char message[5];

  message[0] = FLAG;
  message[1] = A;
  message[2] = C_UA;
  message[3] = A ^ C_UA;
  message[4] = FLAG;

  res = write(fd, message, 5);

  if(res == -1) {
    fprintf(stderr, "Error writing to serial port\n");
    exit(-3);
  }

  else if(res != strlen(message) + 1) {
    fprintf(stderr, "Error: could not send the whole message!\n");
    exit(-4);
  }

  printf("Message sent\n");

/* 
  O ciclo WHILE deve ser alterado de modo a respeitar o indicado no gui�o 
*/

  sleep(2);
  tcsetattr(fd, TCSANOW, &oldtio);
  close(fd);
  
  return 0;
}
