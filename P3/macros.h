
#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A_EE 0x03 //campo de endereco em comandos enviados pelo emissor e respostas enviadas pelo recetor
#define A_ER 0x01 //campo de endereco em comandos enviados pelo recetor e repostas enviadas pelo emissor
#define C_SET 0x03
#define C_UA 0x07
#define C_DISC 0x0B
#define NS0 0x00 //valor de ns na primeira trama do write
#define Ns1 0x40 //valor de ns segunda trama do write
#define ESCAPE_BYTE 0x7d 
#define ESCAPE_FLAG 0x5e
#define ESCAPE_ESCAPE 0x5d
#define RR0 0x05
#define RR1 0x85
#define REJ0 0x01
#define REJ1 0x81

#define TRANSMITTER 0 
#define RECEIVER 1

#define ERROR -1
#define MAXTRIES 3
#define TIMEOUT 7 //Time to wait fora  repsonse from the receiver

extern int stop;