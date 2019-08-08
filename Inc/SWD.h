#ifndef _SWD_H
#define _SWD_H
#include <stdint.h>
#include <stdio.h>

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

typedef enum {
  R0,
  R1,
  R2,
  R3,
  R4,
  R5,
  R6,
  R7,
  R8,
  R9,
  R10,
  R11,
  R12,
  R13,
  R14,
  R15,
} CoreRegister;

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

//Nested Vectored Interrupt Controller
#define CORTEX_AIRCR				0xE000ED0C

//Application Interrupt and Reset Control Register
#define VECTKEY						(0xFA05 << 16)
#define VECTKEYSTAT										// for read (Reads as 0xFA05)
#define ENDIANESS					(1 << 15)			// big endian, 0 = little endian
#define SYSRESETREQ					(1 << 2)
#define VECTCLRACTIVE				(1 << 1)			// clear all state information for active NMI, fault, and interrupts
#define VECTRESET					(1 << 0)			// reset system

//core debug
#define CORTEX_DHCSR				0xE000EDF0
#define CORTEX_DCRSR				0xE000EDF4
#define CORTEX_DCRDR				0xE000EDF8
#define CORTEX_DEMCR				0xE000EDFC

//core debug reg (DHCSR)
#define DBGKEY						(0xA05F << 16)		//Debug Key
#define S_RESET_ST					(1 << 25)
#define S_RETIRE_ST					(1 << 24)
#define S_LOCKUP					(1 << 19)
#define S_SLEEP						(1 << 18)
#define S_HALT						(1 << 17)
#define S_REGRDY					(1 << 16)
#define C_SNAPSTALL					(1 << 5)
#define C_MASKINTS					(1 << 3)
#define C_STEP						(1 << 2)
#define C_HALT						(1 << 1)
#define C_DEBUGEN					(1 << 0)

//core debug reg (DCRSR)
#define REG_WRITE					(1 << 16)
#define REG_READ					(0 << 16)
/*#define R0							0
#define R1							1
#define R2							2
#define R3							3
#define R4							4
#define R5							5
#define R6							6
#define R7							7
#define R8							8
#define R9							9
#define R10							10
#define R11							11
#define R12							12
#define R13							13
#define R14							14
#define R15							15*/

//core debug reg (DEMCR)
#define TRCENA						(1 << 24)
#define VC_HARDERR					(1 << 10)
#define VC_INTERR					(1 << 9)
#define VC_BUSERR					(1 << 8)
#define VC_STATERR					(1 << 7)
#define VC_CHKERR					(1 << 6)
#define VC_NOCPERR					(1 << 5)
#define VC_MMERR					(1 << 4)
#define VC_CORERESET				(1 << 0)

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
uint32_t swdSystemResetAndHaltCore(void);
uint32_t swdReadCoreReg(CoreRegister reg);
void swdWriteCoreReg(CoreRegister reg, uint32_t data);

//Export Variable
extern uint32_t sel;

#endif // _SWD_H
