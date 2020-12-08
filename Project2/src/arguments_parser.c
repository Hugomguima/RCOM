#include "arguments_parser.h"

// ftp://[<user>:<password>@]<host>/<url-path>
int parse_arguments(char* url, arguments *args) {
    char *ftp = strtok(url, "/"); //ftp:
    char *first = strtok(NULL, "/"); //[<user>:<password>@]<host>
    char *path = strtok(NULL, ""); //<url-path>

    if(strcmp(ftp, "ftp:") != 0) {
        printf("Not using ftp protocol\n");
        return -1;
    }

    char *user = strtok(first, ":");
    char *password = strtok(NULL, "@");

    if(password == NULL) { //No block <user>:<password>
        user = "anonymous";
        password = "anonymous";
        args->host_name = first;
    }
    else {
        args->host_name = strtok(NULL, "");
    }

    args->user = user;
    args->password = password;
    args->path = path;

    char copy[1024];
    strcpy(copy, path);
    char *temp = strtok(copy, "/");
    while (temp != NULL)
    {
        args->file_name = temp;
        temp = strtok(NULL, "/");
    }
    
    return 0;
}

int getIP(char *ip, char *host) {
    struct hostent *h;
    
    if((h=gethostbyname(host)) == NULL) {  
        herror("gethostbyname");
        return -2;
    }

    strcpy(ip, inet_ntoa(*((struct in_addr *)h->h_addr)));
    /*printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));*/

    return 0;
}