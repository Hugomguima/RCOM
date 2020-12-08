#include "stateMachines.h"

unsigned char rcv;
int expectedTrama = 0;
int res;


int sendMessage(int fd, unsigned char c){

  unsigned char message[5];

  message[0] = FLAG;
  message[1] = A_EE;
  message[2] = c;
  message[3] = A_EE ^ c;
  message[4] = FLAG;

  return write(fd, message, 5);

}


int readSetMessage(int fd) {
    enum state current = START;

    int finish = FALSE;
    unsigned char r;

    while (finish == FALSE){
        res = read(fd, &r, 1);

        switch (current){
        case START:
            if (r == FLAG){
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if (r == A_EE){
                current = A_RCV;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
            }
            else{
                current = START;
            }
            break;
        case A_RCV:
            if (r == C_SET){
                current = C_RCV;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
            }
            else{
                current = START;
            }
            break;
        case C_RCV:
            if (r == (C_SET ^ A_EE)){
                current = BCC_OK;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
            }
            else{
                current = START;
            }
            break;
        case BCC_OK:
            if (r == FLAG){
                finish = TRUE;
            }
            else{
                current = START;
            }
            break;
        default:
            break;
        }
    }
    return finish;
}

int readReceiverMessage(int fd) {
    enum state current = START;

    int finish = FALSE;
    unsigned char r, check;

    while (finish == FALSE){
        res = read(fd, &r, 1);

        if(res == 0) {
            finish = TRUE;
        }

        switch (current){
        case START:
            if (r == FLAG){
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if (r == A_EE){
                current = A_RCV;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
            }
            else{
                current = START;
            }
            break;
        case A_RCV:
            if (r == REJ0 || r == REJ1 || r == RR0 || r == RR1 ){
                current = C_RCV;
                check = r;
                rcv = r;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
            }
            else{
                current = START;
            }
            break;
        case C_RCV:
            if (r == (check ^ A_EE)){
                current = BCC_OK;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
            }
            else{
                current = START;
            }
            break;
        case BCC_OK:
            if (r == FLAG){
                finish = TRUE;
            }
            else{
                current = START;
            }
            break;
        default:
            break;
        }
    }
    return finish;
}

int receiveUA(int fd){
    unsigned char c; // char read. Changes the state
    int nr;
    enum state current = START;

    int STP = FALSE;
    while(STP == FALSE){
      nr = read(fd,&c,1);

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
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if(c == A_EE){
                current = A_RCV;
            }
            else if(c == FLAG) {
                current = FLAG_RCV;
            }
            else{
                current = START;
            }
            break;
        case A_RCV:
            if(c == C_UA){
                current = C_RCV;
            }
            else if (c == FLAG){
                current = FLAG_RCV;
            }
            else {
                current = START;
            } 
            break;
        case C_RCV:
            if(c == (C_UA ^ A_EE)){
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

  //puts("exiting state machine");
  return TRUE;

}


int receiverRead_StateMachine(int fd, unsigned char* frame, unsigned int *size) { 
    
    unsigned char buf, check = 0;
    int trama = 0;
    enum state current = START;
    int correctBCC2 = FALSE; // if no errors in BCC2, correctBCC2 = 1; else correctBCC2 = 0
    int errorOnDestuffing = FALSE; // if no errors occur on destuffing, the var stays equal to 0, else the value is 1

    puts("Receiver reading frames");
    while(current != STOP) {
        
        res = read(fd, &buf, 1);
        printf("read : 0x%X\n", buf);

        if(res == ERROR) {
            fprintf(stderr, "llread() - Error reading from buffer");
            return -1;
        }

        switch (current)
        {
        case START:
            if(buf == FLAG) {
                current = FLAG_RCV;
            }
            break;

        case FLAG_RCV: 
            if(buf == A_EE) {
                current = A_RCV;
            }
            else if(buf != FLAG) {
                current = START;
            }
            break;
        
        case A_RCV: 
            if(buf == NS0) {
                current = C_RCV;
                check = buf;
                trama = 0;
            }

            else if(buf == NS1) {
                current = C_RCV;
                check = buf;
                trama = 1;
            }

            else if(buf == FLAG) {
                current = FLAG_RCV;
            }

            else {
                current = START;
            }
            break;

        case C_RCV:
            if(buf == (A_EE ^ check)) {
                current = BCC_OK;
            }

            else if(buf == FLAG) {
                current = FLAG_RCV;
            }

            else {
                current = START;
            }
            break;

        case BCC_OK:
            if(buf == FLAG) {
                if(checkBCC2(frame, *size) != ERROR) {
                    correctBCC2 = TRUE;
                    current = STOP;
                }
                else {
                    correctBCC2 = FALSE;
                    current = STOP;
                }
            }
            else if(buf == ESCAPE_BYTE) {
                current = BYTE_DESTUFFING;
            }
            else {
                (*size)++;
                frame = (unsigned char *)realloc(frame, *size);
                frame[*size - 1] = buf; // still receiving data
                
			}

            break;
            
        case BYTE_DESTUFFING:
            if(buf == ESCAPE_FLAG) {
                frame = (unsigned char *)realloc(frame, ++(*size));
				frame[*size - 1] = FLAG;
            }

            else if(buf == ESCAPE_ESCAPE) {
                frame = (unsigned char *)realloc(frame, ++(*size));
                frame[*size - 1] = ESCAPE_BYTE;
            }

            else {
               puts("Character after escape character not recognized"); //can occur if there is an interference
                errorOnDestuffing = TRUE;
            }

            current = BCC_OK;
            break;
        
        default:
            break;
        }
    }

    printf("total size: %d\n",*size);
    frame = (unsigned char *)realloc(frame, *size-1);
	*size = *size - 1;

    printf("Expected trama: %i\n", expectedTrama);
    printf("Received trama: %i\n", trama);
    
    if(correctBCC2 && !errorOnDestuffing) {
        if(trama == expectedTrama) {
            if(trama == 0) {
                //send RR(Nr = 1)
                sendMessage(fd, RR1);
                puts("Receiver send RR1");
            }
            
            else {
                //send RR(Nr = 0)
                sendMessage(fd, RR0);
                puts("Receiver send RR0");
            }
            
            expectedTrama = (expectedTrama + 1) % 2;
        }

        else { 
            *size = 0;

            if(expectedTrama == 0) {  
                //send RR(Nr = 0)
                sendMessage(fd, RR0);
                puts("Receiver send RR0 after repeated information");
            }
            else {
                //send RR(Nr = 1)
                sendMessage(fd, RR1);
                puts("Receiver send RR1 after repeated information");
            }
        }
    }
    else { //caso BCC2 tenha erros ou tenha havido interferencias
        if(trama != expectedTrama) { //verificar informaçao repetida
            if(trama == 0) {
                //send RR(Nr = 1)
                sendMessage(fd, RR1);
                expectedTrama = 1;
                puts("Receiver send RR1 after errors in BCC2");
            }
            else {
                //send RR(Nr=0)
                sendMessage(fd, RR0);
                expectedTrama = 0;
                puts("Receiver send RR0 after errors in BCC2");
            }
        }

        else { //trama correta, mas com erro em BCC2
            *size = 0;

            if(trama == 0) {
                //send REJ 0
                sendMessage(fd, REJ0);
                expectedTrama = 0;
                puts("Receiver send REJ0");
            }

            else {
                //send REJ1
                sendMessage(fd, REJ1);
                expectedTrama = 1;
                puts("Receiver send REJ1");
            }
        }
    }
    return 0;
}

int receiveDISC(int fd) {
    //tcflush(fd, TCIOFLUSH); //limpa informacao recebida mas nao lida e informacao escrita mas nao transmitida

    enum state current = START;

    int finish = FALSE;
    unsigned char r;

    while (finish == FALSE)
    {
        res = read(fd, &r, 1);

        if(res == 0) {
            finish = TRUE;
        }

        switch (current)
        {
        case START:
            if (r == FLAG){

                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if (r == A_EE){
                current = A_RCV;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
            }
            else{
                current = START;
            }
            break;
        case A_RCV:
            if (r == C_DISC){
                current = C_RCV;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
            }
            else{
                current = START;
            }
            break;
        case C_RCV:
            if (r == (C_DISC ^ A_EE)){
                current = BCC_OK;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
            }
            else{
                current = START;
            }
            break;
        case BCC_OK:
            if (r == FLAG){
                finish = TRUE;
            }
            else{
                current = START;
            }
            break;
        default:
            break;
        }
    }
    return 0;
}

int checkBCC2(unsigned char *packet, int size){
    int i;
    unsigned char byte = packet[0];

    for(i = 1; i < size - 1; i++) {
        byte ^= packet[i];
    }

    if(byte == packet[size - 1]) {
        return TRUE;
    }
    else{
        return ERROR;
    }
        
}
