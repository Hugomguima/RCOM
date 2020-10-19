/*Non-Canonical Input Processing*/
#include "emissor.h"

struct termios oldtio,newtio;
volatile int STP=FALSE;
int counter = 0;

void sendMessage(int fd,unsigned char c){

    unsigned char message[5];

    message[0] = FLAG;
    message[1] = A;
    message[2] = c;
    message[3] = A ^ c;
    message[4] = FLAG;

    tcflush(fd,TCIOFLUSH);

    int wr = write(fd,message,5);

    printf("%c message sent: %d \n",c,wr);
}

int receiveUA(int serialPort){

    unsigned char c; // char read. Changes the state
    unsigned char check = 0;
    int nr;


    //Colocar aqui o código da espera pelo byte
    int STP = FALSE;
    while(STP == FALSE){
      puts("Awaiting byte");
      nr = read(serialPort,&c,1);

      if (nr < 0) {
      if (errno == EINTR) {
        puts("Timed out. Sending again.");
        return ERROR;
      }
      continue;
    }

    //State Machine
    switch(current){
        case START:
            if(c == FLAG){
                puts("Flag Received");
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if(c == A){
                puts("A Received");
                current = A_RCV;
                check ^= c;
            }
            else if(c == FLAG);
            else{
                current = START;
            }
            break;
        case A_RCV:
            if(c == C_UA){ //testa agora
                puts("C_SET Received");
                current = C_RCV;
            }
            else if (c == FLAG){
                current = FLAG_RCV;
            }
            else current = START;
            break;
        case C_RCV:
            if(c == BCC_OK){
                puts("BCC ok");
                current = BCC_OK;
            }
            else if( c == FLAG){
              current = FLAG_RCV;
            }
            else{
              current = START;
            }
            break;
        case BCC_OK:
          if(c == FLAG){
            puts("Set received");
            current = STOP;
            STP = TRUE;
          }
          else{
            current = START;
          }
          break;
        case STOP:
          break;
        default:
          break;
      }
    }


  puts("exiting state machine");
  return 0;

}

void llopen(int fd){
    
    // passou para llfunctions.c
    // if ( tcgetattr(fd,&oldtio) == -1) { 
    //   perror("tcgetattr");
    //   exit(-1);
    // }

    // bzero(&newtio, sizeof(newtio));
    // newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    // newtio.c_iflag = IGNPAR;
    // newtio.c_oflag = 0;


    // newtio.c_lflag = 0;

    // newtio.c_cc[VTIME]    = 0;   
    // newtio.c_cc[VMIN]     = 5;   


    // tcflush(fd, TCIOFLUSH);

    // if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    //   perror("tcsetattr");
    //   exit(-1);
    // }

    // printf("New termios structure set\n");


    while(STP == FALSE && counter < MAXTRIES){

      sendMessage(fd,C_SET);
      alarm(TIMEOUT);

      if(receiveUA(fd) == 0){
        printf("Interaction received\n");
        STP = TRUE;
        counter = 0;
      }
      alarm(0);

    }

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

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
