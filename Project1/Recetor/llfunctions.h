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

/**
 * \brief Deals with the protocol initiation establishment
 * @param fd file descriptor for the serial port to be used for the connection
 * @param status If 0, sends SET message and waits for UA, if 1, waits for set and sends UA
 * @return returns 0 upon sucess, -1 otherwise
 */
int llopen(int fd, int status);


/**
 * \brief gets BCC2
 * @param message gets BCC2 from this message
 * @param size message size
 * @return returns BCC2
 */
unsigned char getBCC2(unsigned char *mensagem, int size);

/**
 * \brief stuffs BCC2
 * @param bcc2 bcc2 char to be stuffed
 * @param size size of BCC2 after stuffing
 * @return returns the stuffed BCC2
 */
unsigned char* stuffBCC2(unsigned char bcc2,unsigned int *size);

/**
 * \brief Sends an I packet from a message from buffer to the serial port
 * @param fd fiel desriptor of the serial port
 * @param buffer containing the messsage to be sent
 * @param length length of the message to be sent
 * @return TRUE(1) upon sucess, FALSE(0) upon failure
 */
int llwrite(int fd, unsigned char *buffer, int length);

/**
 * \brief Reads an I packets sent trough the serial port
 * @param fd file descriptor for the serial port
 * @param buffer buffer read from the serial port
 * @return size of the read buffer
 */
unsigned int llread(int fd, unsigned char *buffer);

/**
 * \brief Termination of the protocol by serial port
 * @param fd file descriptor of the serial port
 * @param status if 0, acts as sender. if 1, acts as receiver for the termination protocl
 * @return returns 0 upon sucess, -1 otherwise
 */
int llclose(int fd, int status);

/**
 * \brief handles the alarm
 * @param signo signal number to be handled
 */
void alarmHandler(int signo);