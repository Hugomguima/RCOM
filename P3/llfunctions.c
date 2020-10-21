#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#include "stateMachines.h"


#define TRANSMITTER 0
#define RECEIVER 1
#define BAUDRATE B38400
#define FALSE 0
#define TRUE 1

struct termios oldtio,newtio;

int llopen(int fd, int status) {
    if(status == TRANSMITTER) {
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

        printf("New termios structure set\n");

        //falta enviar a mensagem SET e esperar pela mensagem UA, com alarm
    }
    else if(status == RECEIVER) {
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

        if(readSetMessage(fd) == TRUE) {
            printf("READ SET MESSAGE CORRECTLY");
            if(sendUAMessage(fd) == -1) {
                fprintf(stderr, "Error writing to serial port\n");
                return -1;
            }
            else {
                print("SEND UA MESSAGE");
            }

        }
    }
    return 0;
}

int llwrite(int fd, char *buffer, int lenght) {
    // escreve a trama e fica a espera de receber uma mensagem RR ou REJ para saber o que enviar a seguir
}

int llread(int fd, char *buffer) {
    // le a trama
    // tramas I, S ou U com cabecalho errado são ignoradas, sem qualquer acao
    // caso trama I recebida sem erros detetados no cabecalho e no campo de dados:
    // caso seja uma nova trama, a trama é aceite e passada à aplicação, e envia-se RR para o emissor para confirmar
    // caso seja duplicado, descarta-se a trama e envia-se RR para o emissor 
    // casos trama I sem erro no cabecalho mas com erro detetado pelo BCC no campo de dados:
    // caso seja uma nova trama, a informacao e descartada mas envia-se REJ para o emissor para pedir a retransmissao
    // caso seja duplicado, confirma-se com RR para o transmissor
}

int llclose(int fd) {
    //emissor:
    // envia DISC, espera por DISC e envia UA

    //recetor:
    // le a mensagem DISC enviada pelo emissor, envia DISC e recebe UA
}