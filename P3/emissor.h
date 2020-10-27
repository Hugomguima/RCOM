#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include "macros.h"

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
 * \brief main function that starts the proggram flow
 * @param argc argument count
 * @param argv char pointer array with the arguments
 */
int main(int argc,char** argv);