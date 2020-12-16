#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "arguments_parser.h"



int initConnection(char *ip, int port, int *sockfd);

int receiveAnswer(char *buffer);

int sendData(int sockfd, char *command);

int parseIP_Port(char* buffer, char *ip, int *port);

int downloadFile(int socketfd, char * filename);

#endif