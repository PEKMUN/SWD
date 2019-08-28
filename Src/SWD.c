#include "SWD.h"
#include "swdLowLevel.h"
#include "CException.h"
#include "Exception.h"
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>

uint32_t sel = 0x0;

void swdWriteBits(uint64_t data, int numOfBit)
{
  uint64_t callNum;
  uint64_t tempData=0;
  for(callNum=0 ; callNum < numOfBit ; callNum++)
  {
    tempData = (data >> callNum) & 1;
    swdWriteBit(tempData);
  }
}

uint64_t swdReadBits(int numOfBit)
{
	uint64_t data = 0;
	int k;
  
	for(k=0 ; k<numOfBit ; k++)
		data |= swdReadBit() << k;
  
	return data;
}

void swdResetIntoSwdMode()
{
  swdWriteTurnAround();
  swdWriteBits(0x3ffffffffffff, 50);
  swdWriteBits(0xe79e, 16);
  swdWriteBits(0x3ffffffffffff, 50);
  swdWriteBits(0, 3);
}

int parityCheck(uint32_t data)
{
	uint8_t swdRequest;
	volatile uint8_t parity, ACK;
	int i, numOfOnes=0, temp;
	/*
	 * If the number of '1's is even then write 0,
	 * else write 1.
	*/
	for(i=0 ; i<32 ; i++)
	{
	temp = (data >> i) & 0x1;
		if(temp == 1)
			numOfOnes++;
	}
	if(numOfOnes%2 == 0)
		parity = 0;
	else
		parity = 1;

	return parity;
}

uint64_t swdReadPacket(uint8_t APorDP, uint8_t addr)
{
  uint32_t data;
  uint8_t swdRequest;
  volatile uint8_t parity, checkParity;
  uint8_t ACK;
  
  if(APorDP == 0x1)
  {
    if(((addr & 0xc) >> 2) == 0x0 || ((addr & 0xc) >> 2) == 0x3)
      parity = 0;
    else
      parity = 1;
  }
  else
  {
    if(((addr & 0xc) >> 2) == 0x0 || ((addr & 0xc) >> 2) == 0x3)
      parity = 1;
    else
      parity = 0;
  }
  
  swdRequest = (0b10000101) | (APorDP << 1) | ((addr & 0xc) << 1) | (parity << 5);
  
  swdWriteBits(swdRequest, 8);
  swdReadTurnAround();
  ACK = swdReadBits(3);
  if (ACK != 0b001)
  {
    switch(ACK)
    {
      case 0b010:
        throwError(2, "swdReadPacket. Error: ACK respond WAIT for the transfer.");
        break;
      case 0b100:
        throwError(4, "swdReadPacket. Error: ACK respond FAULT for the transfer.");
        break;
    }
  }
  data = swdReadBits(32);
  checkParity = parityCheck(data);
  parity = swdReadBits(1);
  if(checkParity != parity)
	  throwError(0, "Wrong parity bit detected!");
  swdWriteTurnAround();
  swdWriteBits(0, 3);
  
  return data;
}

void swdWritePacket(uint8_t APorDP, uint8_t addr, uint32_t data)
{
  uint8_t swdRequest;
  volatile uint8_t parity, ACK;
  int i, numOfOnes, temp;
  
  if(APorDP == 0x1)
  {
    if(((addr & 0xc) >> 2) == 0x0 || ((addr & 0xc) >> 2) == 0x3)
      parity = 1;
    else
      parity = 0;
  }
  else
  {
    if(((addr & 0xc) >> 2) == 0x0 || ((addr & 0xc) >> 2) == 0x3)
      parity = 0;
    else
      parity = 1;
  }
  
  swdRequest = (0b10000001) | (APorDP << 1) | ((addr & 0xc) << 1) | (parity << 5);
  
  swdWriteBits(swdRequest, 8);
  swdReadTurnAround();
  ACK = swdReadBits(3);
  if (ACK != 0b001)
  {
    switch(ACK)
    {
      case 0b010:
        throwError(2, "swdWritePacket. Error: ACK respond WAIT for the transfer.");
        break;
      case 0b100:
        throwError(1, "swdWritePacket. Error: ACK respond FAULT for the transfer.");
        break;
    }
  }
  swdWriteTurnAround();
  swdWriteBits(data, 32);
  parity = parityCheck(data);
  swdWriteBits(parity, 1);
  swdWriteBits(0, 3);
}

uint32_t swdReadAP(uint8_t ApSel, uint8_t addr)
{
	volatile uint32_t data;
	uint8_t bankSel, temp;
	temp = addr;
	bankSel = temp & 0xf0;
	if((sel & 0xf0) != ((ApSel << 24) | bankSel))
	{
		sel = (sel & 0x00ffff0f) | bankSel | (ApSel << 24);
		swdWritePacket(DP, SW_DP_SELECT, sel);
	}
	data = swdReadPacket(AP, addr);
	data = swdReadPacket(DP, SW_DP_RDBUFF);
	return data;
}

void swdWriteAP(uint8_t ApSel, uint8_t addr, uint32_t data)
{
	uint8_t bankSel, temp;
	temp = addr;
	bankSel = temp & 0xff;
	if((sel & 0xff0000ff) != ((ApSel << 24) | bankSel))
	{
		sel = (sel & 0x00ffff0f) | bankSel | (ApSel << 24);
		swdWritePacket(DP, SW_DP_SELECT, sel);
	}
	swdWritePacket(AP, addr, data);
}

uint32_t swdReadDP(uint8_t addr)
{
	uint32_t data;
	uint8_t bankSel, temp;
	temp = addr;
	bankSel = (temp & 0xf0) >> 4;
	if((sel & 0xf) != bankSel)
	{
		sel = (sel & 0xfffffff0) | bankSel;
		swdWritePacket(DP, SW_DP_SELECT, sel);
	}
	data = swdReadPacket(DP, addr);
	return data;
}

void swdWriteDP(uint8_t addr, uint32_t data)
{
	uint8_t bankSel, temp;
	temp = addr;
	bankSel = (temp & 0xf0) >> 4;
	if((sel & 0xf) != bankSel)
	{
		sel = (sel & 0xfffffff0) | bankSel;
		swdWritePacket(DP, SW_DP_SELECT, sel);
	}
	swdWritePacket(DP, addr, data);
}

uint32_t swdInitTap(void)
{
	volatile uint32_t id, ctrl;
	swdResetIntoSwdMode();
	id = swdReadPacket(DP, SW_DP_DPIDR);
	// must clear STICKYERR bit in CTRL/STAT register using ABORT register before using any AP commands
	swdWritePacket(DP, SW_DP_ABORT, ORUNERRCLR | WDERRCLR | STKERRCLR | STKCMPCLR | DAPABORT);
	ctrl = swdReadDP(SW_DP_CTRL_STAT);
	swdWritePacket(DP, SW_DP_SELECT, sel = 0x00);
	ctrl = swdReadDP(SW_DP_CTRL_STAT);
	return id;
}

void pwrUp_sys_dbg()
{
	uint32_t checkBit;
	swdWriteDP(SW_DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ);
	checkBit = swdReadDP(SW_DP_CTRL_STAT);
	while ((checkBit & 0xA0000000) != (CSYSPWRUPACK | CDBGPWRUPACK));
}

// just call 1 time at main
uint32_t initAHB_AP()
{
	uint32_t IDR_Val, temp;
	pwrUp_sys_dbg();
	IDR_Val = swdReadAP(AHB_AP, AHB_AP_IDR);
	temp = swdReadAP(AHB_AP, AHB_AP_CSW);
	swdWriteAP(AHB_AP, AHB_AP_CSW, temp | CSW_SIZE_2);
	return IDR_Val;
}

uint32_t swdReadMem32(uint32_t addr)
{
	uint32_t data;
	swdWriteAP(AHB_AP, AHB_AP_TAR, addr);
	data = swdReadAP(AHB_AP, AHB_AP_DRW);
	return data;
}

void swdWriteMem32(uint32_t addr, uint32_t data)
{
	swdWriteAP(AHB_AP, AHB_AP_TAR, addr);
	swdWriteAP(AHB_AP, AHB_AP_DRW, data);
}

uint32_t swdSystemResetAndHaltCore(void)
{
	swdWriteMem32(CORTEX_DHCSR, DBGKEY | C_HALT | C_DEBUGEN);
	swdWriteMem32(CORTEX_DEMCR, TRCENA | VC_CORERESET);
	swdWriteMem32(CORTEX_AIRCR, VECTKEY | SYSRESETREQ);
	return swdReadMem32(CORTEX_DHCSR);
}

uint32_t swdReadCoreReg(CoreRegister reg)
{
	swdWriteMem32(CORTEX_DCRSR, REG_READ | reg);
	return swdReadMem32(CORTEX_DCRDR);
}

void swdWriteCoreReg(CoreRegister reg, uint32_t data)
{
	swdWriteMem32(CORTEX_DCRDR, data);
	swdWriteMem32(CORTEX_DCRSR, REG_WRITE | reg);
}
