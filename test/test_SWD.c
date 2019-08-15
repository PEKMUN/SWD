#include <stdint.h>
#include <stdio.h>
#include "unity.h"
#include "CExceptionConfig.h"
#include "Exception.h"
#include "SWD.h"
#include "mock_swdLowLevel.h"
#include "UnityErrorHandler.h"

swdSeq *seqPtr = NULL;
int seqIndex = 0, seqBit = 0;

char *getNameOfFakeSwdState(fakeSwdState state)
{
	switch(state)
	{
		case SWD_UNKNOWN_STATE:
			return "SWD_UNKNOWN_STATE";
			break;
		case SWD_WRITE:
			return "SWD_WRITE";
			break;
		case SWD_READ:
			return "SWD_READ";
			break;
		case SWD_READ_TURN_AROUND:
			return "SWD_READ_TURN_AROUND";
			break;
		case SWD_WRITE_TURN_AROUND:
			return "SWD_WRITE_TURN_AROUND";
			break;
		/*case SWD_SEQ_END:
			return "SWD_SEQ_END";
			break;*/
		default:
			return "SWD BLANK STATE";
	}
}

int swdSeqStateMac(fakeSwdState state, uint64_t data)
{
  int bit = (seqPtr[seqIndex].data >> seqBit) & 1;
  int len = seqPtr[seqIndex].length;
  fakeSwdState operation = seqPtr[seqIndex].operation;
  
  switch(operation)
  {
    case SWD_WRITE:
      if(state == SWD_WRITE)
        if(bit == data)
        {
          seqBit++;
            if(seqBit == len)
					{
						seqIndex++;
						seqBit = 0;
					}
        }
        else
          unityError("On Entry %d. Bit %d expected %d was %d", seqIndex, seqBit, bit, data);
      else
        unityError("On Entry %d. State expected SWD_WRITE was %s", seqIndex, getNameOfFakeSwdState(state));
      break;

		case SWD_READ:
			if(state == SWD_READ)
      {
        seqBit++;
				if(seqBit == len)
				{
					seqIndex++;
					seqBit = 0;
				}
        return bit;
      }
      else
        unityError("On Entry %d. State expected SWD_READ was %s", seqIndex, getNameOfFakeSwdState(state));
      break;
			
		case SWD_READ_TURN_AROUND:
			if(state == SWD_READ_TURN_AROUND)
			{
				seqIndex++;
        //swdReadTurnAround();
			}
      else
        unityError("On Entry %d. State expected SWD_READ_TURN_AROUND was %s", seqIndex, getNameOfFakeSwdState(state));
      break;
      
		case SWD_WRITE_TURN_AROUND:
			if(state == SWD_WRITE_TURN_AROUND)
      {
				seqIndex++;
        //swdWriteTurnAround();
			}
      else
        unityError("On Entry %d. State expected SWD_WRITE_TURN_AROUND was %s", seqIndex, getNameOfFakeSwdState(state));
      break;
      
		default:
			break;
  }
}

void fake_swdWriteBit(int oneBitData, int callNum)
{
  swdSeqStateMac(SWD_WRITE, oneBitData);
  //printf("swdWriteBit (%i) called: %i\n", oneBitData, callNum);
}

int fake_swdReadBit(int callNum)
{
  return swdSeqStateMac(SWD_READ, 0);
	//printf("swdReadBit called: %i\n", callNum);
}

void fake_swdReadTurnAround(int callNum)
{
  swdSeqStateMac(SWD_READ_TURN_AROUND, 0);
	//printf("swdReadTurnAround called: %i\n", callNum);
}

void fake_swdWriteTurnAround(int callNum)
{
  swdSeqStateMac(SWD_WRITE_TURN_AROUND, 0);
	//printf("swdWriteTurnAround called: %i\n", callNum);
}

void swdFakeSeq(swdSeq seq[])
{
  swdWriteBit_StubWithCallback(fake_swdWriteBit);
  swdReadBit_StubWithCallback(fake_swdReadBit);
  swdReadTurnAround_StubWithCallback(fake_swdReadTurnAround);
  swdWriteTurnAround_StubWithCallback(fake_swdWriteTurnAround);
  seqPtr = seq;
  seqIndex = 0;
  seqBit = 0;
}

void verifySwdSequence()
{
	fakeSwdState operation = seqPtr[seqIndex].operation;

	if(operation != SWD_SEQ_END)
		unityError("The sequence stop on entry %d.", seqIndex);
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_swdWriteBits_given_0b11_expect_calling_swdWrite_2_time(void)
{
  swdSeq seq[] = {
    {SWD_WRITE, 0b11, 2},
    {SWD_SEQ_END, 0, 0},
  };
  
  swdFakeSeq(seq);
  
  swdWriteBits(0b11, 2);
	
	verifySwdSequence();
}

/*void test_swdWriteBits_given_0b01_expect_fail(void)
{
  swdSeq seq[] = {
    {SWD_WRITE, 0b11, 2},
    {SWD_SEQ_END, 0, 0},
  };
  
  swdFakeSeq(seq);
  
  swdWriteBits(0b01, 2);
	
	verifySwdSequence();
}*/

void test_swdWriteBits_given_0xabcdef1234_expect_calling_swdWrite_40_time(void)
{
  swdSeq seq[] = {
    {SWD_WRITE, 0xabcdef1234, 40},
    {SWD_SEQ_END, 0, 0},
  };
  
  swdFakeSeq(seq);
  
  swdWriteBits(0xabcdef1234, 40);
	
	verifySwdSequence();
}

void test_swdReadBits_given_0b101_expect_return_5(void)
{
  swdSeq seq[] = {
    {SWD_READ, 0b101, 3},
    {SWD_SEQ_END, 0, 0},
  };
  
  swdFakeSeq(seq);
  
  TEST_ASSERT_EQUAL(5, swdReadBits(3));
	
	verifySwdSequence();
}

void test_swdWriteBits_and_swdReadTurnAround_given_0b10_expect_proper_swd_sequence(void)
{
  swdSeq seq[] = {
    {SWD_WRITE, 0b10, 2},
    {SWD_READ_TURN_AROUND, 0, 0},
    {SWD_SEQ_END, 0, 0},
  };
  
  swdFakeSeq(seq);
  
  swdWriteBits(0b10, 2);
  swdReadTurnAround();
  
	verifySwdSequence();
}

void test_swdReadBits_and_swdWriteTurnAround_given_0b111_expect_proper_swd_sequence(void)
{
  swdSeq seq[] = {
    {SWD_READ, 0b111, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_SEQ_END, 0, 0},
  };
  
  swdFakeSeq(seq);
  
  TEST_ASSERT_EQUAL(7, swdReadBits(3));
  swdWriteTurnAround();
  
	verifySwdSequence();
}

void test_swdReadBits_and_swdWriteTurnAround_given_0b110_expect_proper_swd_sequence(void)
{
  swdSeq seq[] = {
    {SWD_READ, 0b110, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_SEQ_END, 0, 0},
  };
  
  swdFakeSeq(seq);
  
  TEST_ASSERT_EQUAL(6, swdReadBits(3));
  swdWriteTurnAround();
  
	verifySwdSequence();
}

void test_swdWritePacket_given_AP_ADDR_2_DATA_0x12345678_expect_proper_swd_sequence(void)
{
  swdSeq seq[] = {
    {SWD_WRITE, 0xfa, 8},
    {SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0x12345678, 32},
    {SWD_WRITE, 1, 1},
    {SWD_WRITE, 0, 3},
    {SWD_SEQ_END, 0, 0},
  };
  
  // Setup the fake
  swdFakeSeq(seq);
  
  swdWriteBits(0xfa, 8);
  swdReadTurnAround();
  TEST_ASSERT_EQUAL(1, swdReadBits(3));
  swdWriteTurnAround();
  swdWriteBits(0x12345678, 32);
  swdWriteBits(1, 1);
  swdWriteBits(0, 3);
	
	verifySwdSequence();
}

void test_swdResetIntoSwdMode_expect_proper_swd_sequence(void)
{
  swdSeq seq[] = {
    {SWD_WRITE_TURN_AROUND, 0, 0},
		{SWD_WRITE, 0x3ffffffffffff, 50},
		{SWD_WRITE, 0xe79e, 16},
		{SWD_WRITE, 0x3ffffffffffff, 50},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
	
	// Setup the fake
  swdFakeSeq(seq);
	
	swdResetIntoSwdMode();
	verifySwdSequence();
}

void test_swdReadPacket_given_SWD_IDCODE_expect_return_correct_idcode(void)
{
  CEXCEPTION_T ex;
  uint32_t idcode;
  
  swdSeq seq[] = {
		{SWD_WRITE, 0xa5, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
		{SWD_READ, 0x1BA00477, 32},
    {SWD_READ, 1, 1},
    {SWD_WRITE_TURN_AROUND, 0, 0},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    idcode = swdReadPacket(DP, 0b01000000);
    TEST_ASSERT_EQUAL(0x1BA00477, idcode);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdReadPacket_given_SWD_IDCODE_expect_a_fault_return(void)
{
  CEXCEPTION_T ex;
  uint32_t idcode;
  
  swdSeq seq[] = {
		{SWD_WRITE, 0xa5, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b100, 3},
		{SWD_READ, 0x1BA00477, 32},
    {SWD_READ, 1, 1},
    {SWD_WRITE_TURN_AROUND, 0, 0},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    idcode = swdReadPacket(DP, 0b01000000);
    TEST_FAIL_MESSAGE("Expect an error to be thrown.But none.");
  } Catch(ex) {
		printf("%s", ex->errorMsg);
    TEST_ASSERT_EQUAL(ACK_FAULT, ex->errorCode);
  }
}

void test_swdReadPacket_given_SWD_IDCODE_expect_a_wait_return(void)
{
  CEXCEPTION_T ex;
  uint32_t idcode;
  
  swdSeq seq[] = {
		{SWD_WRITE, 0xa5, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b010, 3},
		{SWD_READ, 0x1BA00477, 32},
    {SWD_READ, 1, 1},
    {SWD_WRITE_TURN_AROUND, 0, 0},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    idcode = swdReadPacket(DP, 0b01000000);
    TEST_FAIL_MESSAGE("Expect an error to be thrown.But none.");
  } Catch(ex) {
		printf("%s", ex->errorMsg);
    TEST_ASSERT_EQUAL(ACK_WAIT, ex->errorCode);
  }
}

void test_swdWritePacket_given_0xbad0face_expect_proper_swd_sequence(void)
{
  CEXCEPTION_T ex;
  
  swdSeq seq[] = {
		{SWD_WRITE, 0b10011001, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0xbad0face, 32},
    {SWD_WRITE, 1, 1},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    swdWritePacket(DP, 0b00001100, 0xbad0face);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdWritePacket_given_0xbad0face_expect_a_fault_return(void)
{
  CEXCEPTION_T ex;
  
  swdSeq seq[] = {
		{SWD_WRITE, 0b10011001, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b100, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0xbad0face, 32},
    {SWD_WRITE, 1, 1},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    swdWritePacket(DP, 0b00001100, 0xbad0face);
    TEST_FAIL_MESSAGE("Expect an error to be thrown.But none.");
  } Catch(ex) {
    printf("%s", ex->errorMsg);
    TEST_ASSERT_EQUAL(ACK_FAULT, ex->errorCode);
  }
}

void test_swdWritePacket_given_0xbad0face_expect_a_wait_return(void)
{
  CEXCEPTION_T ex;
  
  swdSeq seq[] = {
		{SWD_WRITE, 0b10011001, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b010, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0xbad0face, 32},
    {SWD_WRITE, 1, 1},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    swdWritePacket(DP, 0b00001100, 0xbad0face);
    TEST_FAIL_MESSAGE("Expect an error to be thrown.But none.");
  } Catch(ex) {
    printf("%s", ex->errorMsg);
    TEST_ASSERT_EQUAL(ACK_WAIT, ex->errorCode);
  }
}

void test_swdReadDP_given_SWD_IDCODE_expect_return_correct_idcode(void)
{
  CEXCEPTION_T ex;
  uint32_t idcode;
  
  swdSeq seq[] = {
    // Park | Stop | Parity | A3 | A2 | R/W | AP/DP | Start
    //  1   |  0   |        |    |    | 1/0 |  1/0  | 1
		{SWD_WRITE, 0xa5, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
		{SWD_READ, 0x1BA00477, 32},
    {SWD_READ, 1, 1},
    {SWD_WRITE_TURN_AROUND, 0, 0},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
	sel = 0x0;
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    idcode = swdReadDP(0x0);
		TEST_ASSERT_EQUAL(0x0, sel);
    TEST_ASSERT_EQUAL(0x1BA00477, idcode);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdReadDP_given_0xfc_expect_return_0x12345678(void)
{
  CEXCEPTION_T ex;
  uint32_t ret_val;
  
  swdSeq seq[] = {
    // SWD Write Packet
    // Park | Stop | Parity | A3 | A2 | R/W | AP/DP | Start
    //  1    |  0   |         |     |     | 1/0  |  1/0		|		1
    {SWD_WRITE, 0b10110001, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0xff00000f, 32},
    {SWD_WRITE, 0, 1},
		{SWD_WRITE, 0, 3},
    // SWD Read Packet
		{SWD_WRITE, 0b10111101, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
		{SWD_READ, 0x12345678, 32},
    {SWD_READ, 1, 1},
    {SWD_WRITE_TURN_AROUND, 0, 0},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
	sel = 0xff000000;
	
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    ret_val = swdReadDP(0xfc);
		TEST_ASSERT_EQUAL(0xff00000f, sel);
    TEST_ASSERT_EQUAL(0x12345678, ret_val);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdWriteDP_given_SW_DP_CTRL_STAT_expect_proper_swd_sequence(void)
{
  CEXCEPTION_T ex;
  
  swdSeq seq[] = {
    {SWD_WRITE, 0xa9, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0x50000000, 32},
    {SWD_WRITE, 0, 1},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
	sel = 0x0;
	
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    swdWriteDP(SW_DP_CTRL_STAT, 0x50000000);
		TEST_ASSERT_EQUAL(0x0, sel);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdWriteDP_given_SWD_IDCODE_or_0x2_expect_proper_swd_sequence(void)
{
  CEXCEPTION_T ex;
  
  swdSeq seq[] = {
		// write packet 1
		{SWD_WRITE, 0b10110001, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0xff000000, 32},
    {SWD_WRITE, 0, 1},
		{SWD_WRITE, 0, 3},
		// write packet 2
    {SWD_WRITE, 0b10000001, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0x12345678, 32},
    {SWD_WRITE, 1, 1},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
	sel = 0xff000000 | 1;
	
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    swdWriteDP(0x2, 0x12345678);
		TEST_ASSERT_EQUAL(0xff000000, sel);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdReadAP_given_AHB_AP_IDR_expect_return_correct_IDR_value(void)
{
  CEXCEPTION_T ex;
  uint32_t IDR_val;
  
  swdSeq seq[] = {
    // SWD Write Packet
    {SWD_WRITE, 0xb1, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0xf0, 32},
    {SWD_WRITE, 0, 1},
		{SWD_WRITE, 0, 3},
    // SWD Read Packet
		{SWD_WRITE, 0x9f, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
		{SWD_READ, 0x24770011, 32},
    {SWD_READ, 0, 1},
    {SWD_WRITE_TURN_AROUND, 0, 0},
		{SWD_WRITE, 0, 3},
    // SWD Read Packet
		{SWD_WRITE, 0xbd, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
		{SWD_READ, 0x24770011, 32},
    {SWD_READ, 0, 1},
    {SWD_WRITE_TURN_AROUND, 0, 0},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
	sel = 0x0;
	
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    IDR_val = swdReadAP(0, AHB_AP_IDR);
		TEST_ASSERT_EQUAL(0xf0, sel);
    TEST_ASSERT_EQUAL(0x24770011, IDR_val);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdReadAP_given_AHB_AP_IDR_and_select_equal_0xf0_expect_return_correct_IDR_value(void)
{
  CEXCEPTION_T ex;
  uint32_t IDR_val;
  
  swdSeq seq[] = {
    // Read packet 1
		{SWD_WRITE, 0x9f, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
		{SWD_READ, 0x24770011, 32},
    {SWD_READ, 0, 1},
    {SWD_WRITE_TURN_AROUND, 0, 0},
		{SWD_WRITE, 0, 3},
		// Read packet 2
    {SWD_WRITE, 0xbd, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
		{SWD_READ, 0x24770011, 32},
    {SWD_READ, 0, 1},
    {SWD_WRITE_TURN_AROUND, 0, 0},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
	sel = 0xf0;
	
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    IDR_val = swdReadAP(0, AHB_AP_IDR);
		TEST_ASSERT_EQUAL(0xf0, sel);
    TEST_ASSERT_EQUAL(0x24770011, IDR_val);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdWriteAP_given_AHB_AP_CSW_expect_proper_swd_sequence(void)
{
  CEXCEPTION_T ex;
  
  swdSeq seq[] = {
    {SWD_WRITE, 0xa3, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0x2, 32},
    {SWD_WRITE, 1, 1},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
	sel = 0x0;
	
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    swdWriteAP(0, AHB_AP_CSW, 0x2);
		TEST_ASSERT_EQUAL(0x0, sel);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdWriteAP_given_AHB_AP_CSW_and_select_0xff000034_expect_proper_swd_sequence(void)
{
  CEXCEPTION_T ex;
  
  swdSeq seq[] = {
		// Write packet 1
		{SWD_WRITE, 0b10110001, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0x00000004, 32},
    {SWD_WRITE, 1, 1},
		{SWD_WRITE, 0, 3},
		// Write packet 2
    {SWD_WRITE, 0xa3, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0x2, 32},
    {SWD_WRITE, 1, 1},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
	sel = 0xff000034;
	
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    swdWriteAP(0, AHB_AP_CSW, 0x2);
		TEST_ASSERT_EQUAL(0x00000004, sel);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdWriteAP_given_select_0x0200002a_expect_return_same_value(void)
{
  CEXCEPTION_T ex;
  
  swdSeq seq[] = {
    {SWD_WRITE, 0b10010011, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0x12345678, 32},
    {SWD_WRITE, 1, 1},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
	sel = (0x2 << 24) | (0x2 << 4) | 0xa;
	
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    swdWriteAP(0x2, (0x2 << 24) | (0x2 << 4) | 0xa, 0x12345678);
		TEST_ASSERT_EQUAL(0x0200002a, sel);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}

void test_swdWriteAP_given_select_0x0200002a_and_write_APSEL_0x1_expect_select_equal_0x0100002a(void)
{
  CEXCEPTION_T ex;
  
  swdSeq seq[] = {
		// Write packet 1
		{SWD_WRITE, 0b10110001, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0x0100002a, 32},
    {SWD_WRITE, 0, 1},
		{SWD_WRITE, 0, 3},
		// Write packet 2
    {SWD_WRITE, 0b10010011, 8},
		{SWD_READ_TURN_AROUND, 0, 0},
    {SWD_READ, 0b001, 3},
    {SWD_WRITE_TURN_AROUND, 0, 0},
    {SWD_WRITE, 0x12345678, 32},
    {SWD_WRITE, 1, 1},
		{SWD_WRITE, 0, 3},
		{SWD_SEQ_END, 0, 0},
  };
  
	sel = (0x2 << 24) | (0x2 << 4) | 0xa;
	
  // Setup the fake
  swdFakeSeq(seq);
  
  Try {
    swdWriteAP(0x1, (0x2 << 4) | (0x2 << 2), 0x12345678);
		TEST_ASSERT_EQUAL(0x0100002a, sel);
  } Catch(ex) {
    TEST_FAIL_MESSAGE(ex->errorMsg);
  }
}