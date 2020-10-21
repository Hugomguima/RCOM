#include <termios.h>

#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07

int readSetMessage(int fd) {
    tcflush(fd, TCIOFLUSH); //limpa informacao recebida mas nao lida e informacao escrita mas nao transmitida

    enum state
    {
        START,
        FLAG_RCV,
        A_RCV,
        C_RCV,
        BCC_OK,
        STOP
    };
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
            if (r == A)
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