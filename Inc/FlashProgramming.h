#ifndef _FLASHPROGRAMMING_H
#define _FLASHPROGRAMMING_H
#include <stdint.h>
#include <stdio.h>

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

void swdWriteFlash(uint8_t targetFlashAddr, uint8_t *data, int len);

#endif // _FLASHPROGRAMMING_H