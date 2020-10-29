#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"

unsigned char* openFile(unsigned char *filename, off_t *fileSize);

unsigned char* parseControlPacket(unsigned int state, off_t fileSize, unsigned char* filename, int sizeFileName, int *sizeControlPacket);

unsigned char* parseDataPacket(unsigned char *message, off_t fileSize, int *packetSize);