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

/**
 * \brief codifies the Given message into a packet according to the protocol
 * @param message message to be sent
 * @param fileSize total size of the file to be written
 * @param length total size of the packet to be sent through llwrite serial port
 */
unsigned char* parseDataPacket(unsigned char *message, off_t fileSize, int *length);


/**
 * \brief Splits the data into packets that fit into a message (currently set to 128 bytes)
 * @param message message containing the whole data
 * @param index index to start/continue to write the data from
 * @param packetSize returns the ammount of bytes that can be written in a single llwrite (128 or less if end of file reached)
 * @param filesize file total size to check how many bytes should be written
 * @return returns the packet data that will be sent
 */
unsigned char* splitPacket(unsigned char *message,off_t *index, int *packetSize, off_t fileSize);


/**
 * \brief Checks if the first packet read from the sender is indeed the control start packet
 * @param start packet read (first packet)
 * @param filesize gets the total size of the file through the control packet
 * @param name gets the filename through the control packet
 * @param nameSize gets the filename size through the control packet
 */
int checkStart(unsigned char* start, unsigned int *filesize, char *name, unsigned int *nameSize);


/**
 * \brief Checks if the control END packet is equal to the START control packet
 * @param start start packet read (first packet)
 * @param startSize size of the start packet
 * @param end end packet read (last packet)
 * @param endSize size of the end packet
 * @return 0 if both packets are equal, 1 otherwise
 */
int checkEND(unsigned char *start, int startSize, unsigned char *end, int endSize);

/**
 * \brief Creates a the packet to be sent trough llwrite serial port
 * @param message Received message to be sent
 * @param messageSize size of the received message
 * @param packetSize size of the created packet to be returned
 * @return Assembled packet to be sent
 */
unsigned char* assembleDataPacket(unsigned char* message, unsigned int messageSize, unsigned int *packetSize);

/**
 * \brief Reads a file given a name and data
 * @param data file data
 * @param fileSize size of the file to be created
 * @param filename name of the file to be created
 */
void createFile(unsigned char* data, unsigned int fileSize, char *filename);