/*Non-Canonical Input Processing*/
#include "emissor.h"

struct termios oldtio,newtio;
volatile int STP=FALSE;
int counter = 0;

int sendMessage(int fd,unsigned char c){

  unsigned char message[5];

  message[0] = FLAG;
  message[1] = A;
  message[2] = c;
  message[3] = A ^ c;
  message[4] = FLAG;

  tcflush(fd,TCIOFLUSH);

  return write(fd,message,5);

}


void alarmHandler(int signo){

  puts("Entered Alarm handler");
  counter++;
  if(counter >= MAXTRIES){
    printf("Exceeded maximum amount of tries: (%d)\n",MAXTRIES);
  }
  return;
}

int main(int argc, char** argv)
{
  int fd,c;
  char *buf = NULL;
  int i, sum = 0, speed = 0;

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

  // Installing Alarm Handler

  if(signal(SIGALRM,alarmHandler) || siginterrupt(SIGALRM,1)){
      printf("Signal instalation failed");
  }

  // Dealing with the SET and UA
  llopen(fd);

  close(fd);
  return 0;
}
