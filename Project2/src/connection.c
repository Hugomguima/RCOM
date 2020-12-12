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
        memset(buffer, 0, 512);
        buffer = fgets(buffer, 512, socketFile);
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

    return 0;
}

int parseIP_Port(char* buffer, char *ip, int *port) {
    strtok(buffer, "(");
    char *ip1 = strtok(NULL, ",");
    char *ip2 = strtok(NULL, ",");
    char *ip3 = strtok(NULL, ",");
    char *ip4 = strtok(NULL, ",");
    char *port1 = strtok(NULL, ",");
    char *port2 = strtok(NULL, ")");

    sprintf(ip, "%s.%s.%s.%s", ip1, ip2, ip3, ip4);
    *port = atoi(port1) * 256 + atoi(port2);

    return 0;
}

int downloadFile(int socketfd, char * filename) {
    printf("> filename: %s\n", filename);

    int fd = open(filename, O_WRONLY | O_CREAT, 0777);

    if(fd < 0) {
        printf("FD: %i\n", fd);
        printf("Error creating file\n");
        return -5;
    }

    char buffer[1];
    int numBytes;

    while((numBytes = read(socketfd, buffer, 1)) > 0) {
        if(write(fd, buffer, numBytes) < 0) {
            printf("Error writing to file\n");
            return -6;
        }
    }

    if(close(fd) < 0) {
        printf("Error closing file\n");
        return -7;
    }

    return 0;
}
