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

  //tcflush(fd,TCIOFLUSH);

  return write(fd, message, 5);

}


int readSetMessage(int fd) {
    //tcflush(fd, TCIOFLUSH); //limpa informacao recebida mas nao lida e informacao escrita mas nao transmitida

    
    enum state current = START;

    int finish = FALSE;
    unsigned char r, check;

    while (finish == FALSE){
        res = read(fd, &r, 1);

        switch (current){
        case START:
            if (r == FLAG){
                //puts("Flag Received");
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if (r == A_EE){
                //puts("A Received");
                current = A_RCV;
                check ^= r;
            }
            else if (r == FLAG){
                //puts("Received a FLAG on FLAG_RCV");
                current = FLAG_RCV;
            }
            else{
                current = START;
                //puts("Return to START on FLAG_RCV");
            }
            break;
        case A_RCV:
            if (r == C_SET){
                current = C_RCV;
                check ^= r;
                //puts("C Received");
                rcv = r;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
                //puts("Received a FLAG on A_RCV");
            }
            else{
                current = START;
                //puts("Return to START on A_RCV");
            }
            break;
        case C_RCV:
            if (r == check){
                //puts("BCC OK");
                current = BCC_OK;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
                //puts("Received a FLAG on C_RCV");
            }
            else{
                current = START;
                //puts("Return to START on C_RCV");
            }
            break;
        case BCC_OK:
            if (r == FLAG){
                //puts("SET correct");
                finish = TRUE;
            }
            else{
                current = START;
                //puts("Return to START on BCC_OK");
            }
            break;
        default:
            break;
        }
    }
    return finish;
}

int readReceiverMessage(int fd) {
    //tcflush(fd, TCIOFLUSH); //limpa informacao recebida mas nao lida e informacao escrita mas nao transmitida

    
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
                //puts("Flag Received");
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if (r == A_EE){
                //puts("A Received");
                current = A_RCV;
                check ^= r;
            }
            else if (r == FLAG){
                //puts("Received a FLAG on FLAG_RCV");
                current = FLAG_RCV;
            }
            else{
                current = START;
                //puts("Return to START on FLAG_RCV");
            }
            break;
        case A_RCV:
            if (r == REJ0 || r == REJ1 || r == RR0 || r == RR1 ){
                current = C_RCV;
                check ^= r;
                //puts("C Received");
                rcv = r;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
                //puts("Received a FLAG on A_RCV");
            }
            else{
                current = START;
                //puts("Return to START on A_RCV");
            }
            break;
        case C_RCV:
            if (r == check){
                //puts("BCC OK");
                current = BCC_OK;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
                //puts("Received a FLAG on C_RCV");
            }
            else{
                current = START;
                //puts("Return to START on C_RCV");
            }
            break;
        case BCC_OK:
            if (r == FLAG){
                //puts("SET correct");
                finish = TRUE;
            }
            else{
                current = START;
                //puts("Return to START on BCC_OK");
            }
            break;
        default:
            break;
        }
    }
    return finish;
}

int receiveUA(int fd){
    //tcflush(fd, TCIOFLUSH);

    unsigned char c; // char read. Changes the state
    unsigned char check = 0;
    int nr;
    enum state current = START;


    //Colocar aqui o código da espera pelo byte
    int STP = FALSE;
    while(STP == FALSE){
      //puts("Awaiting byte");
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
                //puts("Flag Received");
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if(c == A_EE){
                //puts("A Received");
                current = A_RCV;
                check ^= c;
            }
            else if(c == FLAG) {
                //puts("Received a FLAG on FLAG_RCV");
                current = FLAG_RCV;
            }
            else{
                current = START;
                //puts("Return to START on FLAG_RCV");
            }
            break;
        case A_RCV:
            if(c == C_UA){ //testa agora
                //puts("C_SET Received");
                current = C_RCV;
            }
            else if (c == FLAG){
                //puts("Received a FLAG on A_RCV");
                current = FLAG_RCV;
            }
            else {
                current = START;
                //puts("Return to START on A_RCV");
            } 
            break;
        case C_RCV:
            if(c == BCC_OK){
                //puts("BCC OK");
                current = BCC_OK;
            }
            else if( c == FLAG){
              current = FLAG_RCV;
              //puts("Received a FLAG on C_RCV");
            }
            else{
              current = START;
              //puts("Return to START on C_RCV");
            }
            break;
        case BCC_OK:
          if(c == FLAG){
            //puts("UA received");
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
    int correctBCC2 = 0; // if no errors in BCC2, correctBCC2 = 1; else correctBCC2 = 0
    int errorOnDestuffing = 0; // if no errors occur on destuffing, the var stays equal to 0, else the value is 1

    while(current != STOP) {
        puts("Receiver reading frames");
        res = read(fd, &buf, 1);

        printf("read : 0x%X\n", buf);

        if(res == -1) {
            fprintf(stderr, "llread() - Error reading from buffer");
            return -1;
        }

        switch (current)
        {
        case START:
            if(buf == FLAG) {
                current = FLAG_RCV;
                puts("Reading frames: FLAG Received");
            }
            break;

        case FLAG_RCV: 
            if(buf == A_EE) {
                current = A_RCV;
                check ^= buf;
                printf("check =0x%X\n",check);
                puts("Reading frames: A Received");
            }

            else if(buf == FLAG) {
                puts("Reading frames: Received FLAG on FLAG_RCV");
            }

            else {
                current = START;
                puts("Reading frames: Return to START on FLAG_RCV");
            }
            break;
        
        case A_RCV: 
            //como fazer caso seja informaçao repetida?
            if(buf == NS0) {
                current = C_RCV;
                check ^= buf;
                trama = 0;
                puts("Reading frames: C Received trama 0");
            }

            else if(buf == NS1) {
                current = C_RCV;
                check ^= buf;
                trama = 1;
                puts("Reading frames: C Received trama 1");
            }

            else if(buf == FLAG) {
                current = FLAG_RCV;
                puts("Reading frames: Received a FLAG on A_RCV");
            }

            else {
                current = START;
                puts("Reading frames: Return to START on A_RCV");
            }
            break;

        case C_RCV:
            if(buf == check) {
                current = BCC_OK;
                puts("Reading frames: BCC OK");
            }

            else if(buf == FLAG) {
                current = FLAG_RCV;
                puts("Reading frames: Received a FLAG in C_RCV");
            }

            else {
                current = START;
                puts("Reading frames: Return to START on C_RCV");
            }
            break;

        case BCC_OK:
            if(buf == FLAG) {
                if(checkBCC2(frame, *size) == 0) {
                    correctBCC2 = 1;
                    current = STOP;
                    puts("Receiver: frame with correct BCC2");
                }
                else {
                    correctBCC2 = 0;
                    current = STOP;
                    puts("Reading frames: Errors on BCC2");
                }
            }
            else if(buf == ESCAPE_BYTE) {
                current = BYTE_DESTUFFING;
                puts("Reading frames: Needs destuffing");
            }
            else {
                (*size)++;
                printf("size2 = %d\n",*size);
                frame = (unsigned char *)realloc(frame, *size);
                puts("realloc worked");
                frame[*size - 1] = buf; // still receiving data
                
			}

            break;
            
        case BYTE_DESTUFFING:
            if(buf == ESCAPE_FLAG) {
                frame = (unsigned char *)realloc(frame, ++(*size));
				frame[*size - 1] = FLAG;
                puts("Reading frames: Destuffing done");
            }

            else if(buf == ESCAPE_ESCAPE) {
                frame = (unsigned char *)realloc(frame, ++(*size));
                frame[*size - 1] = ESCAPE_BYTE;
                puts("Reading frames: Destuffing done");
            }

            else {
                printf("Character after escape character not recognized\n"); //can occur if there is an interference
                errorOnDestuffing = 1;
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
    
    if(correctBCC2 == 1 && errorOnDestuffing == 0) {
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
            printf("expected trama: %d\n",expectedTrama);
        }

        else { //ao fazermos isto ja garantimos que a trama recebida e repetida?
            *size = 0;

            if(expectedTrama == 0) {  //acho que é isto ou faz-se com trama?
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
        if(trama != expectedTrama) { //sera isto suficiente para verificar informaçao repetida?
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
    unsigned char r, check;

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
                //puts("Flag Received");
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if (r == A_EE){
                //puts("A Received");
                current = A_RCV;
                check ^= r;
            }
            else if (r == FLAG){
                //puts("Received a FLAG on FLAG_RCV");
                current = FLAG_RCV;
            }
            else{
                current = START;
                //puts("Return to START on FLAG_RCV");
            }
            break;
        case A_RCV:
            if (r == C_DISC){
                //puts("C_DISC Received");
                current = C_RCV;
                check ^= r;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
                //puts("Received a FLAG on A_RCV");
            }
            else{
                current = START;
                //puts("Return to START on A_RCV");
            }
            break;
        case C_RCV:
            if (r == check){
                //puts("BCC OK");
                current = BCC_OK;
            }
            else if (r == FLAG){
                current = FLAG_RCV;
                //puts("Received a FLAG on C_RCV");
            }
            else{
                current = START;
                //puts("Return to START on C_RCV");
            }
            break;
        case BCC_OK:
            if (r == FLAG){
                //puts("SET correct");
                finish = TRUE;
                //return C_DISC;
            }
            else{
                current = START;
                //puts("Return to START on BCC_OK");
            }
            break;
        default:
            break;
        }
    }
    return 0;
}

int checkBCC2(unsigned char *packet, int size) {
    int i;
    unsigned char byte = packet[0];

    for(i = 1; i < size - 1; i++) {
        byte ^= packet[i];
    }

    if(byte == packet[size - 1]) {
        return 0;
    }
    else 
        return 1;

}
