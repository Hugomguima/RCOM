#include <termios.h>
#include <stdio.h>
#include "stateMachines.h"
#include <errno.h>
#include "macros.h"
#include "emissor.h"

unsigned char rcv;


int readSetMessage(int fd) {
    tcflush(fd, TCIOFLUSH); //limpa informacao recebida mas nao lida e informacao escrita mas nao transmitida

    
    enum state current = START;

    int finish = FALSE;
    unsigned char r, check;

    while (finish == FALSE && current != STOP){
        read(fd, &r, 1);

        switch (current)
        {
        case START:
            if (r == FLAG)
            {
                puts("Flag Received");
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if (r == A_EE)
            {
                puts("A Received");
                current = A_RCV;
                check ^= r;
            }
            else if (r == FLAG)
            {
                puts("still a flag");
            }
            else
            {
                current = START;
            }
            break;
        case A_RCV:
            if (r == C_SET || r == REJ0 || r == REJ1 || r == RR0 || r == RR1 )
            {
                current = C_RCV;
                check ^= r;
                puts("Received C");
                rcv = r;
            }
            else if (r == FLAG)
            {
                current = FLAG_RCV;
                puts("Recebe Flag novamente");
            }
            else
            {
                current = START;
                puts("volta para o start");
            }
            break;
        case C_RCV:
            if (r == check)
            {
                puts("BCC OK");
                current = BCC_OK;
            }
            else if (r == FLAG)
            {
                current = FLAG_RCV;
            }
            else
            {
                current = START;
            }
            break;
        case BCC_OK:
            if (r == FLAG)
            {
                puts("SET correct");
                finish = TRUE;
            }
            else
            {
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
    tcflush(fd, TCIOFLUSH);

    unsigned char c; // char read. Changes the state
    unsigned char check = 0;
    int nr;
    enum state current = START;


    //Colocar aqui o código da espera pelo byte
    int STP = FALSE;
    while(STP == FALSE){
      puts("Awaiting byte");
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
                puts("Flag Received");
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if(c == A_EE){
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
  return TRUE;

}


int receiverRead_StateMachine(int fd, unsigned long *size) { //nao sei se size leva pointer ou nao
    unsigned char buf, check;
    int res, finish = FALSE, trama = 0, expectedTrama = 0;
    enum state current = START;
    unsigned char *frame = (unsigned char *)malloc(0);
    int correctBCC2 = 0; // if no errors in BCC2, correctBCC2 = 1; else correctBCC2 = 0
    int errorOnDestuffing = 0; // if no errors occur on destuffing, the var stays equal to 0, else the value is 1

    while(finish = FALSE) {
        
        res = read(fd, &buf, 1);

        if(res == -1) {
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
                check ^= buf;
            }

            else if(buf == FLAG) {
                current == FLAG_RCV;
            }

            else {
                current == START;
            }
            break;
        
        case A_RCV: 
            //como fazer caso seja informaçao repetida?
            if(buf == NS0) {
                current = C_RCV;
                check ^= buf;
                trama = 0;
            }

            else if(buf == NS1) {
                current = C_RCV;
                check ^= buf;
                trama = 1;
            }

            else if(buf == FLAG) {
                current = FLAG_RCV;
            }

            else {
                current == START;
            }
            break;

        case C_RCV:
            if(buf == check) {
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
                if(checkBCC2(trama, *size) == 0) { //pointer?
                    correctBCC2 = 1;
                    current = STOP;
                }

                else {
                    correctBCC2 = 0;
                    current = STOP;
                }
            }

            else if(buf == ESCAPE_BYTE) {
                current = BYTE_DESTUFFING;
            }

            else {
                frame = (unsigned char *)realloc(frame, ++(*size)); 
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
                printf("Character after escape character not recognized\n"); //can occur if there is an interference
                errorOnDestuffing = 1;
            }

            current = BCC_OK;
            break;
        
        default:
            break;
        }
    }

    frame = (unsigned char *)realloc(frame, *size-1);
	*size = *size - 1;

    printf("Expected trama: %i", expectedTrama);
    printf("Received trama: %i", trama);
    
    if(correctBCC2 == 1 && errorOnDestuffing == 0) {
        if(trama == expectedTrama) {
            if(trama == 0) {
                //send RR(Nr = 1)
                sendMessage(fd, RR1);
            }
            
            else {
                //send RR(Nr = 0)
                sendMessage(fd, RR0);
            }
            
            expectedTrama = (expectedTrama + 1) % 2;
        }

        else { //ao fazermos isto ja garantimos que a trama recebida e repetida?
            *size = 0;

            if(expectedTrama == 0) {  //acho que é isto ou faz-se com trama?
                //send RR(Nr = 0)
                sendMessage(fd, RR0);
            }

            else {
                //send RR(Nr = 1)
                sendMessage(fd, RR1);
            }
        }
    }
    else { //caso BCC2 tenha erros ou tenha havido interferencias
        if(trama != expectedTrama) { //sera isto suficiente para verificar informaçao repetida?
            if(trama == 0) {
                //send RR(Nr = 1)
                sendMessage(fd, RR1);
                expectedTrama = 1;
            }
            else {
                //send RR(Nr=0)
                sendMessage(fd, RR0);
                expectedTrama = 0;
            }
        }

        else { //trama correta, mas com erro em BCC2
            *size = 0;

            if(trama == 0) {
                //send REJ 0
                sendMessage(fd, REJ0);
                expectedTrama = 0;
            }

            else {
                //send REJ1
                sendMessage(fd, REJ1);
                expectedTrama = 1;
            }
        }
    }
    return ;
}

int receiveDISC(int fd) {
    tcflush(fd, TCIOFLUSH); //limpa informacao recebida mas nao lida e informacao escrita mas nao transmitida

    enum state current = START;

    int finish = FALSE;
    unsigned char r, check;

    while (finish == FALSE)
    {
        read(fd, &r, 1);

        switch (current)
        {
        case START:
            if (r == FLAG)
            {
                puts("Flag Received");
                current = FLAG_RCV;
            }
            break;
        case FLAG_RCV:
            if (r == A_EE)
            {
                puts("A Received");
                current = A_RCV;
                check ^= r;
            }
            else if (r == FLAG)
            {
                puts("still a flag");
            }
            else
            {
                current = START;
            }
            break;
        case A_RCV:
            if (r == C_DISC)
            {
                puts("C_DISC Received");
                current = C_RCV;
                check ^= r;
            }
            else if (r == FLAG)
            {
                current = FLAG_RCV;
                puts("entra aqui");
            }
            else
            {
                current = START;
                puts("volta para o start");
            }
            break;
        case C_RCV:
            if (r == check)
            {
                puts("BCC OK");
                current = BCC_OK;
            }
            else if (r == FLAG)
            {
                current = FLAG_RCV;
            }
            else
            {
                current = START;
            }
            break;
        case BCC_OK:
            if (r == FLAG)
            {
                puts("SET correct");
                finish = TRUE;
                return C_DISC;
            }
            else
            {
                current = START;
            }
            break;
        case STOP:
            break;
        default:
            break;
        }
    }
    return C_DISC;
}

int checkBCC2(unsigned char *packet, int size) {
    int i;
    unsigned char byte = packet[0];

    for(i = 0; i < size; i++) {
        byte ^= packet[i];
    }

    if(byte == packet[size - 1]) {
        return 0;
    }
    else 
        return 1;
}
