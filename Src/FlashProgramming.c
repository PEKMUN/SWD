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
	uint32_t *src = (uint32_t *)swdReadMem32(&monitorState->dataAddr);
	while(monitorState->command==TARGET_NOT_READY);
	for(int i=0 ; i<(len/4) ; i++)
	{
		swdWriteMem32(src, *data);
		data++;
		src++;
	}
	monitorState->flashAddr = targetFlashAddr;
	monitorState->dataSize = len;
	monitorState->command = WRITE_DATA;
}
