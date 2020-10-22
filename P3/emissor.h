#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include <errno.h>
#include <signal.h>
#include <stdlib.h>


#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07


#define ERROR -1
#define MAXTRIES 3
#define TIMEOUT 5

enum state {START,FLAG_RCV,A_RCV,C_RCV,BCC_OK,STOP};
enum state current = START;

/**
 * \brief Sends message C to file descriptor fd
 * @param fd filedescriptor for serial port
 * @param c message specification
 */
int sendMessage(int fd,unsigned char c);

/**
 * \brief handles the alarm
 * @param signo signal number to be handled
 */
void alarmHandler(int signo);

/**
 * \brief State machine for receiving the UA frame
 * @param serialPort file descriptor for the serial port
 */
int receiveUA(int serialPort);

/**
 * \brief main function that starts the proggram flow
 * @param argc argument count
 * @param argv char pointer array with the arguments
 */
int main(int argc,char** argv);