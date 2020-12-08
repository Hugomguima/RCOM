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

    if(parseArguments(argv[1], &args) != 0) {
        printf("Error parsing introduced arguments\n");
        return -2;
    }

    strcpy(fileName, args.file_name);

    if(getIP(ipAddress, args.host_name) != 0) {
        printf("Error getting host IP address\n");
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

    printf("SOCKET FD: %i\n", sockfd);

    return 0;
}