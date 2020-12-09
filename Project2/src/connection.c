#include "connection.h"

int initConnection(char *ip, int port, int *sockfd) {
    struct	sockaddr_in server_addr;

    /*server address handling*/
	bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ((*sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0) {
        perror("socket()");
        return -1;
    }
	/*connect to the server*/
    if(connect(*sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("connect()");
        return -2;
	}

    return 0;
}

int receiveAnswer(char *buffer) {
    do {
        memset(buffer, 0, 256);
        buffer = fgets(buffer, 256, socketFile);
        printf("< %s", buffer);
    } while(buffer[3] != ' ');

    return 0;
}

int sendData(int sockfd, char *command) {
    printf("> %s\n", command);
    int s = send(sockfd, command, strlen(command), 0);

    if(s == 0) {
        printf("sendData: Connection closed\n");
        return -3;
    }
    else if(s == -1) {
        printf("sendData: Error\n");
        return -4;
    }
    
    printf("> Command Sent\n");

    return 0;
}