
enum state {
        START,
        FLAG_RCV,
        A_RCV,
        C_RCV,
        BCC_OK,
        BYTE_DESTUFFING,
        STOP
    };

int readSetMessage(int fd);

int receiverRead_StateMachine(int fd, unsigned long *size);

int receiveUA(int serialPort);

int checkBCC2(unsigned char *packet, int size);