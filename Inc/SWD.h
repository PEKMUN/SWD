#ifndef _SWD_H
#define _SWD_H
#include <stdint.h>
#include <stdio.h>

/*typedef enum {
  SWD_FALLING_EDGE = 0,
  SWD_RISING_EDGE = 1,
} fakeSwdClkState;*/

typedef enum {
  SWD_UNKNOWN_STATE,
  SWD_WRITE,
  SWD_READ,
  SWD_READ_TURN_AROUND,
  SWD_WRITE_TURN_AROUND,
	SWD_SEQ_END,
} fakeSwdState;

typedef struct swdSeq swdSeq;
struct swdSeq {
  fakeSwdState operation;
  uint64_t data;
  int length;
};

#define AP				1
#define DP				0

// Error number
#define WRONG_PARITY	0
#define ACK_WAIT    	2
#define ACK_FAULT   	4

#define AHB_AP			0

// AP
#define AHB_AP_CSW                  0x00
#define AHB_AP_TAR                  0x04
#define AHB_AP_DRW                  0x0C
#define AHB_AP_IDR                  0xFC

// DP
#define SW_DP_ABORT                 0x00
#define SW_DP_DPIDR                 0x00
#define SW_DP_CTRL_STAT             0x04
#define SW_DP_DLCR                  0x14
#define SW_DP_TARGETID              0x24
#define SW_DP_DLPIDR                0x34
#define SW_DP_EVENTSTAT             0x44
#define SW_DP_SELECT                0x08
#define SW_DP_RDBUFF                0x0C

// CTRL/STAT register
#define CSYSPWRUPACK				(1 << 31)
#define CSYSPWRUPREQ				(1 << 30)
#define CDBGPWRUPACK				(1 << 29)
#define CDBGPWRUPREQ				(1 << 28)

// ABORT reg
#define ORUNERRCLR					(1 << 4)
#define WDERRCLR					(1 << 3)
#define STKERRCLR					(1 << 2)
#define STKCMPCLR					(1 << 1)
#define DAPABORT					(1 << 0)

// CSW reg
#define CSW_AddrInc_Inc_Single		(1 << 5)
#define CSW_SIZE_2					(2 << 0)

void fake_swdWriteBit(int oneBitData, int callNum);
int fake_swdReadBit(int callNum);
void fake_swdReadTurnAround(int callNum);
void fake_swdWriteTurnAround(int callNum);
void swdFakeSeq(swdSeq seq[]);
void verifySwdSequence();

int parityCheck(uint32_t data);
void swdWriteBits(uint64_t data, int numOfBit);
uint64_t swdReadBits(int numOfBit);
void swdResetIntoSwdMode();
uint64_t swdReadPacket(uint8_t APorDP, uint8_t addr);
void swdWritePacket(uint8_t APorDP, uint8_t addr, uint32_t data);
uint32_t swdReadAP(uint8_t ApSel, uint8_t addr);
void swdWriteAP(uint8_t ApSel, uint8_t addr, uint32_t data);
uint32_t swdReadDP(uint8_t addr);
void swdWriteDP(uint8_t addr, uint32_t data);
uint32_t swdInitTap(void);
void pwrUp_sys_dbg();
uint32_t initAHB_AP();
uint32_t swdReadMem32(uint32_t addr);
void swdWriteMem32(uint32_t addr, uint32_t data);

//Export Variable
extern uint32_t sel;

#endif // _SWD_H
