#include <sys/types.h>

#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07

int sendUAMessage(int fd)
{
    ssize_t res;
    unsigned char message[5];

    message[0] = FLAG;
    message[1] = A;
    message[2] = C_UA;
    message[3] = A ^ C_UA;
    message[4] = FLAG;

    res = write(fd, message, 5);

    return res;
}