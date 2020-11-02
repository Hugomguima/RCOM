#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

/**
 * \brief opens the file sent and returns its data and size
 * @param filename file to be read
 * @param fileSize returns size of the file after being read
 * @return returns the data of the read file
 */
unsigned char* openFile(char *filename, off_t *fileSize);


/**
 * \brief Generates the control packet for a given file
 * @param state FLAG to distinguish END from START packet. START = 0x02, END = 0x03
 * @param filesize size of the read file
 * @param filename name of the read file
 * @param sizeFileName size of the name of the read file
 * @param sizeControlPacket returns the size of the generatedcontrol packet
 * @return returns the generated control packet
 */
unsigned char* parseControlPacket(unsigned int state, off_t fileSize, char* filename, int sizeFileName, int *sizeControlPacket);

unsigned char* parseDataPacket(unsigned char *message, off_t fileSize, int *packetSize);

unsigned char* splitPacket(unsigned char *message,off_t *index, int *packetSize, off_t fileSize);

int checkStart(unsigned char* start, unsigned int *filesize, char *name, unsigned int *nameSize);

int checkEND(unsigned char *start, int startSize, unsigned char *end, int endSize);

unsigned char* assembleDataPacket(unsigned char* message, unsigned int messageSize, unsigned int *packetSize);

void createFile(unsigned char* data, unsigned int fileSize, char *filename);