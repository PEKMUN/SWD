#ifndef _FLASHPROGRAMMING_H
#define _FLASHPROGRAMMING_H
#include <stdint.h>
#include <stdio.h>

#define KB						1024
#define TRANFER_BUFFER_SIZE		(2*KB)

typedef enum {
  TARGET_NOT_READY,
  TARGET_READY,
  WRITE_DATA,
  MASS_ERASE,
  PAGE_ERASE,
} commandState;

typedef struct FlashState FlashState;
struct FlashState {
	commandState command;
	uint32_t dataAddr;
	int blockSize;
	uint32_t flashAddr;
	int dataSize;
	int sector;
	int numOfSector;
};

extern uint8_t transferBuffer[];
void swdWriteFlash(uint32_t targetFlashAddr, uint8_t *data, int len);

#endif // _FLASHPROGRAMMING_H
