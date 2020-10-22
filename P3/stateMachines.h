
enum state {
        START,
        FLAG_RCV,
        A_RCV,
        C_RCV,
        BCC_OK,
        STOP
    };

int readSetMessage(int fd);

int receiverRead_StateMachine();

int receiveUA(int serialPort);