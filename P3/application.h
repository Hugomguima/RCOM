#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"

unsigned char* openFile(unsigned char *filename, off_t *fileSize);

unsigned char* parseControlPacket(unsigned int state, off_t fileSize, unsigned char* filename, int sizeFileName, int *sizeControlPacket);

unsigned char* parseDataPacket(unsigned char *message, off_t fileSize, int *packetSize);

unsigned char* splitPacket(unsigned char *message,off_t *index, int *packetSize, off_t fileSize);

int checkStart(unsigned char* start, unsigned int *filesize,unsigned char *name, unsigned int *nameSize);

int checkEND(unsigned char *start, int startSize, unsigned char *end, int endSize);

unsigned char* assembleDataPacket(unsigned char* message, unsigned int messageSize, unsigned int *packetSize);

void createFile(unsigned char* data, unsigned int fileSize, unsigned char *filename);