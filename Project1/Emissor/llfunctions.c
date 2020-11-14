#include "llfunctions.h"


struct termios oldtio,newtio;

volatile int STP=FALSE;
extern unsigned char rcv;
int counter = 0;
int trama = 0;
extern int res;


int llopen(int fd, int status) {
    
    if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
        perror("llopen: tcgetattr");
        return ERROR;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 50;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 0 chars received */

    /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        return ERROR;
    }

    puts("New termios structure set");


    if(status == TRANSMITTER) {

        // Installing Alarm Handler
        if(signal(SIGALRM, alarmHandler) || siginterrupt(SIGALRM, 1)){
            puts("Signal instalation failed");
            return ERROR;
        }

        counter = 0;
        do{
            int wr;
            if((wr = sendMessage(fd, C_SET)) != ERROR){
                printf("llopen: C_SET message sent: %d \n", wr);
            }
            else{
                puts("llopen: Error sending message");
            }

            alarm(TIMEOUT); // Call an alarm to wait for the message

            if(receiveUA(fd) == TRUE){
                puts("TRANSMITTER: UA received\n");
                STP = TRUE;
                counter = 0;
                alarm(0);
            }

        }while(STP == FALSE && counter < MAXTRIES);
    }
    else if(status == RECEIVER) {
        if(readSetMessage(fd) == TRUE) {
            puts("RECEIVER: Read SET message correctly");
            if(sendMessage(fd, C_UA) == -1) {
                fprintf(stderr, "llopen - Error writing to serial port (Receiver)\n");
                return ERROR;
            }
            else {
                puts("RECEIVER: Sent UA message");
            }
        }
        else {
            fprintf(stderr, "llopen - Error reading from serial port (Receiver)\n");
            return ERROR;
        }
    }
    return 0;
}

unsigned char getBCC2(unsigned char *mensagem, int size){

    unsigned char bcc2 = mensagem[0];
    
    for(int i = 1; i < size; i++){
        bcc2 ^= mensagem[i];
    }
    return bcc2;
}

unsigned char* stuffBCC2(unsigned char bcc2, unsigned int *size){
    unsigned char* stuffed;
    if(bcc2 == FLAG){
        stuffed = malloc(2 * sizeof(unsigned char));
        stuffed[0] = ESCAPE_BYTE;
        stuffed[1] = ESCAPE_FLAG; 
        (*size) = 2;
    }
    else if(bcc2 == ESCAPE_BYTE){
        stuffed = malloc(2 * sizeof(unsigned char));
        stuffed[0] = ESCAPE_BYTE;
        stuffed[1] = ESCAPE_ESCAPE;
        (*size) = 2; 
    }
    else{
        stuffed = malloc(sizeof(unsigned char));
        stuffed[0] = bcc2;
        (*size) = 1;
    }
    
    return stuffed;
}

int llwrite(int fd, unsigned char *buffer, int length) {
// escreve a trama e fica a espera de receber uma mensagem RR ou REJ para saber o que enviar a seguir
    unsigned char bcc2;
    unsigned int sizebcc2 = 1;
    unsigned int messageSize = length+6;
    unsigned char *bcc2Stuffed = (unsigned char *)malloc(sizeof(unsigned char));
    unsigned char *message = (unsigned char *)malloc(messageSize * sizeof(unsigned char));

    bcc2 = getBCC2(buffer, length);
    bcc2Stuffed = stuffBCC2(bcc2, &sizebcc2);

    
    // Inicio do preenchimento da mensagem
    message[0] = FLAG;
    message[1] = A_EE;
    if(trama == 0){
        message[2] = NS0;
    }
    else{
        message[2] = NS1;
    }
    message[3] = message[1] ^ message[2];

    // Começa a ler do 4 e o tamanho depende da mensagem a ser enviada
    int i = 4;
    for(int j = 0; j < length; j++){
        if(buffer[j] == FLAG){
            message = (unsigned char *)realloc(message, ++messageSize);
            message[i] = ESCAPE_BYTE;
            message[i + 1] = ESCAPE_FLAG;
            i+=2;
        }
        else if(buffer[j] == ESCAPE_BYTE){
            message = (unsigned char *)realloc(message, ++messageSize);
            message[i] = ESCAPE_BYTE;
            message[i+1] = ESCAPE_ESCAPE;
            i+=2;
        }
        else{
            message[i] = buffer[j];
            i++;
        }
    }

    if(sizebcc2 == 2){
        message = (unsigned char *)realloc(message, ++messageSize);
        message[i] = bcc2Stuffed[0];
        message[i + 1]  = bcc2Stuffed[1];
        i+=2;
    }
    else{
        message[i] = bcc2;
        i++;
    }
    message[i] = FLAG;

    //Mensagem preenchida Trama I feita
    // printMessage

    for(int j = 0; j < messageSize; j++){
        printf("message[%d] = 0x%X\n", j, message[j]);
    }
    

    counter = 0;
    STP = FALSE;

    // Envio da trama
    do {
        // Processo de escrita
        //tcflush(fd,TCIOFLUSH);

        unsigned char* copyBcc = (unsigned char *)malloc(messageSize);
        unsigned char* copyBcc2 = (unsigned char *)malloc(messageSize);

        copyBcc = generateRandomBCC(message,messageSize);
        copyBcc2 = generateRandomBCC2(copyBcc,messageSize);

        // Para já ainda não sei qual é o tamanho
        int wr = write(fd, copyBcc2, messageSize);

        printf("TRANSMITTER: SET message sent: %d bytes sent\n", wr);

        alarm(TIMEOUT);

        // Mudar o processo de espera não é receiveUA
        readReceiverMessage(fd);
    
        // Tratar do rcv
        if((rcv == RR0 && trama == 1) || (rcv == RR1 && trama == 0)) {
            counter = 0;
            trama = (trama + 1) % 2;
            STP = FALSE;
            alarm(0);
            if(rcv == RR0) {
                puts("TRANSMITTER: Received RR0");
            }
            else {
                puts("TRANSMITTER: Received RR1");
            }
            break;
        }

        else if(rcv == REJ0 || rcv == REJ1) {
            STP = TRUE;
            //alarm(0);
            if(rcv == REJ0) {
                puts("TRANSMITTER: Received REJ0");
            }
            else {
                puts("TRANSMITTER: Received REJ1");
            }
        }

        else if(res == 0) {
            puts("TRANSMITTER: Don't read any message from Receiver");
            STP = TRUE;
        }

        else {
            puts("TRANSMITTER: Received an invalid message");
        }

    } while(STP && counter < MAXTRIES); 

    if(counter >= MAXTRIES) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

unsigned int llread(int fd, unsigned char* buffer) {
// le a trama
// tramas I, S ou U com cabecalho errado são ignoradas, sem qualquer acao
// caso trama I recebida sem erros detetados no cabecalho e no campo de dados:
// caso seja uma nova trama, a trama é aceite e passada à aplicação, e envia-se RR para o emissor para confirmar
// caso seja duplicado, descarta-se a trama e envia-se RR para o emissor 
// casos trama I sem erro no cabecalho mas com erro detetado pelo BCC no campo de dados:
// caso seja uma nova trama, a informacao e descartada mas envia-se REJ para o emissor para pedir a retransmissao
// caso seja duplicado, confirma-se com RR para o transmissor

    unsigned int size = 0;
    receiverRead_StateMachine(fd,buffer, &size);

    printf("size llread = %d\n",size);

    return size;
}

int llclose(int fd, int status) {
//emissor:
// envia DISC, espera porInteraction received\n
    if(status == TRANSMITTER) {

        counter = 0;
        STP = FALSE;
        
        do {
            int wr;
            if((wr = sendMessage(fd, C_DISC)) != ERROR){
                puts("TRANSMITTER: C_DISC message sent");
            }
            else{
                puts("TRANSMITTER: Error sending C_DISC message");
            }

            alarm(TIMEOUT); // Call an alarm to wait for the message

            if(receiveDISC(fd) == 0 && res != 0){
                puts("TRANSMITTER: C_DISC received");
                STP = TRUE;
                counter = 0;
                alarm(0);
            }
        } while(STP == FALSE && counter < MAXTRIES);

        if(sendMessage(fd, C_UA)) {
            puts("TRANSMITTER: Send UA");
        }
        tcsetattr(fd, TCSANOW, &oldtio);
    }

//recetor:
// le a mensagem DISC enviada pelo emissor, envia DISC e recebe UA
    else if(status == RECEIVER) {
        if (receiveDISC(fd) == 0) {
            puts("RECEIVER: Read DISC");
            if(sendMessage(fd, C_DISC)) {
                puts("RECEIVER: Send DISC");
                if(receiveUA(fd) == TRUE) {
                    puts("RECEIVER: Read UA");
                }
                
                else {
                    fprintf(stderr, "llclose- Error reading UA message (Receiver)\n");
                    return ERROR;
                }
            }

            else {
                fprintf(stderr, "llclose- Error writing DISC message to serial port (Receiver)\n");
                return ERROR;
            }
        }

        else {
            fprintf(stderr, "llclose - Error reading DISC message (Receiver)\n");
            return ERROR;
        }
        tcsetattr(fd, TCSANOW, &oldtio);
    }
    return 0;
}



void alarmHandler(int signo){

  counter++;
  if(counter >= MAXTRIES){
    printf("Exceeded maximum amount of tries: (%d)\n", MAXTRIES);
    exit(0);
  }
  return ;
}



unsigned char* generateRandomBCC(unsigned char* packet, int packetSize){
    unsigned char* copy = (unsigned char *)malloc(packetSize);
    memcpy(copy,packet,packetSize);

    if(((rand() % 100) + 1 ) <= BCC1ERRORRATE){
        unsigned char hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

        copy[(rand() % 3) + 1] = hex[rand() % 16];
        puts("BCC Value sucessfully changed");
    }
    return copy;
}


unsigned char* generateRandomBCC2(unsigned char* packet, int packetSize){
    unsigned char* copy = (unsigned char *)malloc(packetSize);
    memcpy(copy,packet,packetSize);

    if(((rand() % 100) + 1 ) <= BCC2ERRORRATE){
        unsigned char hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
        
        copy[(rand() % (packetSize - 5)) + 4] = hex[rand() % 16];
        puts("BCC2 Value sucessfully changed");
    }
    return copy;
}