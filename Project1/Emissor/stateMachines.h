#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>


#include "macros.h"


enum state {
        START,
        FLAG_RCV,
        A_RCV,
        C_RCV,
        BCC_OK,
        BYTE_DESTUFFING,
        STOP
    };

int sendMessage(int fd,unsigned char c);

int readSetMessage(int fd);

int readReceiverMessage(int fd);

int receiveUA(int serialPort);

int receiverRead_StateMachine(int fd, unsigned char* frame ,unsigned int *size);

int receiveDISC(int fd);

int checkBCC2(unsigned char *packet, int size);