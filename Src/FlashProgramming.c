#include "FlashProgramming.h"
#include "swdLowLevel.h"
#include "SWD.h"
#include "CException.h"
#include "Exception.h"
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

uint8_t transferBuffer[TRANFER_BUFFER_SIZE];
FlashState *monitorState = 0x200001b4;

void swdWriteFlash(uint32_t targetFlashAddr, uint32_t *data, int len)
{
	volatile commandState currentCmd;
	uint32_t *src = (uint32_t *)swdReadMem32(&monitorState->dataAddr);
	do {
		currentCmd = swdReadMem32(&monitorState->command);
	} while(currentCmd==TARGET_NOT_READY);
	for(int i=0 ; i<len/4 ; i++)
	{
		swdWriteMem32(src, data);
		data++;
		src++;
	}
	swdWriteMem32(&monitorState->flashAddr, targetFlashAddr);
	swdWriteMem32(&monitorState->dataSize, len);
	swdWriteMem32(&monitorState->command, WRITE_DATA);
	currentCmd = swdReadMem32(&monitorState->command);
}
