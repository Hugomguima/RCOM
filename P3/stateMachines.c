#include <termios.h>
#include <stdio.h>
#include "stateMachines.h"
#include <errno.h>
#include "macros.h"


int readSetMessage(int fd) {
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
            if (r == C_SET)
            {
                puts("C_SET Received");
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
    return TRUE;
}


int receiverRead_StateMachine(int fd) {
    unsigned char buf, check;
    int res, finish = FALSE, trama = 0;
    enum state current = START;

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
            if(buf == NS0) {
                current = C_RCV;
                check ^= buf;
                trama = 0;
            }

            else if(buf == Ns1) {
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
            

        default:
            break;
        }
    }
}


int receiveUA(int serialPort){

    unsigned char c; // char read. Changes the state
    unsigned char check = 0;
    int nr;
    enum state current = START;


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
  return 0;

}