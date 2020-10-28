#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include "stateMachines.h"
#include "macros.h"

int llopen(int fd, int status);

unsigned char getBCC2(unsigned char *mensagem, int size);

unsigned char* stuffBCC2(unsigned char bcc2,unsigned int *size);

int llwrite(int fd, unsigned char *buffer, int length);

unsigned int llread(int fd, unsigned char *buffer);

int llclose(int fd, int status);

/**
 * \brief handles the alarm
 * @param signo signal number to be handled
 */
void alarmHandler(int signo);