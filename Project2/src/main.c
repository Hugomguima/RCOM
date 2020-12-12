#include <stdio.h>
#include <string.h>
#include "arguments_parser.h"
#include "connection.h"


int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

    arguments args;
    char ipAddress[256];
    char fileName[128];
    int sockfd;
    char answerBuffer[512];
    char command[512];

    if(parseArguments(argv[1], &args) != 0) {
        printf("Error parsing introduced arguments\n");
        return -2;
    }

    strcpy(fileName, args.file_name);

    if(getIP(ipAddress, args.host_name) != 0) {
        printf("< Error getting host IP address\n");
        return -3;
    }

    printf("USER: %s\n", args.user);
    printf("PASS: %s\n", args.password);
    printf("HOST: %s\n", args.host_name);
    printf("PATH: %s\n", args.path);
    printf("FILENAME: %s\n", fileName);
    printf("IP ADDRESS: %s\n", ipAddress);

    if(initConnection(ipAddress, 21, &sockfd) != 0) {
        printf("Error starting connection to server\n");
        return -4;
    }

    socketFile = fdopen(sockfd, "r");
    receiveAnswer(answerBuffer);

    if(answerBuffer[0] == '2') {
        printf("\n< Expecting username...\n");
    }
    else {
        printf("< Error in socket connection\n");
        return -5;
    }

    sprintf(command, "USER %s\r\n", args.user);
    if(sendData(sockfd, command) != 0) {
        return -6;
    }
    receiveAnswer(answerBuffer);

    if(answerBuffer[0] == '3') {
        printf("< Expecting password...\n");
    }
    else {
        printf("< Error sending username\n");
        return -7;
    }

    sprintf(command, "PASS %s\r\n", args.password);
    if(sendData(sockfd, command) != 0) {
        return -8;
    }
    receiveAnswer(answerBuffer);

    if(answerBuffer[0] == '2') {
        printf("< Logged in\n\n");
    }
    else {
        printf("< Error sending password\n");
        return -9;
    }

    char *new_ip = malloc(16);
    int new_port;
    int new_sockfd;

    sprintf(command, "PASV \r\n");
    if(sendData(sockfd, command) != 0) {
        return -8;
    }
    receiveAnswer(answerBuffer);

    if(parseIP_Port(answerBuffer, new_ip, &new_port) != 0) {
        return -9;
    }

    printf("< New IP: %s\n", new_ip);
    printf("< New port: %i\n", new_port);

    if(initConnection(new_ip, new_port, &new_sockfd) != 0) {
        printf("Error starting connection to server\n");
        return -10;
    }

    sprintf(command, "RETR %s\r\n", args.path);
    if(sendData(sockfd, command) != 0) {
        return -11;
    }

    receiveAnswer(answerBuffer);

    if(downloadFile(new_sockfd, fileName) != 0) {
        printf("Error downloading file\n");
        return -12;
    }

    printf("< File %s downloaded\n", fileName);

    if(close(sockfd) < 0) {
        printf("Error closing socket\n");
        return -13;
    }

    if(close(new_sockfd) < 0) {
        printf("Error closing data socket\n");
        return -14;
    }

    //printf("BUF: %s\n", answerBuffer);

    return 0;
}