#include "FlashProgramming.h"
#include "swdLowLevel.h"
#include "SWD.h"
#include "CException.h"
#include "Exception.h"
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

FlashState *monitorState = 0x200001b4;

void swdWriteFlash(uint8_t targetFlashAddr, uint8_t *data, int len)
{
  uint32_t dataAddress = swdReadMem32(&monitorState->dataAddr);
  swdWriteMem32(dataAddress, data);
  data = swdReadMem32(dataAddress);
  swdWriteMem32(targetFlashAddr, data);
}
