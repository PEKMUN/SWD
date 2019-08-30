# SWD Probe and Flash Programmer for STM32F103
This project is to develop a SWD probe that can halt, run, and step the STM32F103 MCU. It is able to read/write memory and core registers. The probe is able to perform flash programming to download program into the target MCU. Both the SWD probe and the target MCU are STM32F103C8T6 microcontroller.

## Prerequisites
**Hardware:**

Blue Pill STM32 MCU - [link](https://jeelabs.org/img/2016/DSC_5474.jpg)
![Blue Pill STM32 MCU](https://jeelabs.org/img/2016/DSC_5474.jpg)

STM32 Smart V2 MCU - [link](https://stm32-base.org/assets/img/boards/STM32F103C8T6_STM32_Smart_V2.0-1.jpg)
![STM32 Smart V2 MCU](https://stm32-base.org/assets/img/boards/STM32F103C8T6_STM32_Smart_V2.0-1.jpg)

**Software:**
- System Workbench for STM32
- STM32CubeMX
- STM32 ST-LINK Utility
- Notepad++

## Circuit Connection
![](https://github.com/PEKMUN/SWD/blob/master/image/circuit.jpg)
- First MCU is PC
- Second MCU is SWD probe
- Last MCU is target

## Setup in STM32CubeMX
![](https://github.com/PEKMUN/SWD/blob/master/image/cubeMX.jpg)

**Pin Configuration:**

![](https://github.com/PEKMUN/SWD/blob/master/image/pin_configuration.JPG)
- PB13 is for SWCLK pin
- PB14 is for SWDIO pin

## 1. SWD Protocol
**Write Packet:**

![](https://github.com/PEKMUN/SWD/blob/master/image/Write_Packet.jpg)

**Read Packet:**

![](https://github.com/PEKMUN/SWD/blob/master/image/Read_Packet.jpg)

- Write Bits: Inside this function will call the low level write bit. The write bit will follow the timing diagram to perform the job.
- Read Bits: Inside this function will call the low level read bit. The read bit will follow the timing diagram to perform the job.
- Read Turn Around: This is a half cycle duration of SWD packet. It happen before acknowledge. This function is used to change swdio from output to input.
- Write Turn Around: This is a one and half SWDCK clock cycle in the SWD packet. It happen after acknowledge. This function is to change swdio from input to output.

**Packet request phase:**
This phase consist of 8 bits.
- `START` One bit. This bit is always 1.
- `APnDP` One bit. 0 if it is DP access and 1 if it is AP access.
- `RnW` One bit. if 1 then it is read access and 0 is write access.
- `A[2:3]` Two bits. Which register is selected for the transaction.
  ![](https://github.com/PEKMUN/SWD/blob/master/image/DP_register.JPG)
- `Parity` One bit. This bit should be 1 if number of 1's in bit `START`+`APnDP`+`RnW`+`A[2:3]` is an odd number. If the number of 1's are even, the parity bit should be zero.
- `Stop` One bit. This bit is always 0.
- `Park` One bit. This bit is always 1.

**Acknowledge:**

![](https://github.com/PEKMUN/SWD/blob/master/image/ack.JPG)

**Dummy Phase:**
The last three bits at Read Packet and Write Packet are very important. This three bits must be write end of the packet to let the target knows it is end of the transaction.

### Reset into SWD
This function is to make sure it is in SWD mode not JTAG mode.
**Steps for JTAG switch to SWD:**
1. Send 50 bits of 1. This step is to ensure it is in the reset mode.
2. Send 16 bits of JTAG-to-SWD sequence bit. The 16 bits JTAG-to-SWD sequence bit is 0xE79E. 
3. Send 50 bits of 1. This step is to ensure the programming spec is already in SWD mode.
4. Send 3 bits of 0 represent the dummy phase.

## 2. Memory Access Port(MEM-AP)
**Before using AHB_AP:**
1. Set CDBGPWRUPREQ and CSYSPWRUPREQ bit in CTRL/STAT register.
2. Write 0xF0 into SELECT register.
3. Read the value of IDR register.
4. Check the value get from previous step is it 0x14770011.
5. Write 0x0 into SELECT register.
6. Set 0x2 in the SIZE field of CSW register.
7. Can access the inrternal memory by using TAR or DRW reister.

## 3. System Reset and Halt Core
There are two reason for doing this:
1. Get the CPU and peripherals into a known state
2. Prohibit the CPU from accidentally running partial code while writing the program

**The process is as follows:**
1. Set DBGKEY, C_HALT and C_DEBUGEN bit in DHCSR register. This will halt the core.
2. Set bit VC_CORERESET in DEMCR register. This will enable halt-on-reset
3. Set VECTKEY and SYSRESETREQ bit in AIRCR register. This will reset the core and halt.

Now the CPU will be halted on the first instruction and all peripherals. Registers (except for the debug registers) will have their reset value.

## 4. Core Register
**Read Core Register:**
1. Set REG_READ and write which register that want to read in DCRSR register.
2. Return the value that read in the DCRDR register.

**Write Core Register:**
1. Write the data into DCRDR register.
2. Set REG_WRITE bit and write which register that want to write in DCRSR register.

## 5. Flash Programming
Refer the flash monitor [here](https://github.com/PEKMUN/FLASH_MONITOR)

## 6. Reference
- https://www.st.com/content/ccc/resource/technical/document/reference_manual/59/b9/ba/7f/11/af/43/d5/CD00171190.pdf/files/CD00171190.pdf/jcr:content/translations/en.CD00171190.pdf
- https://static.docs.arm.com/ihi0031/c/IHI0031C_debug_interface_as.pdf
- https://www.cypress.com/file/44676/download
- https://www.cypress.com/file/136281/download
- https://www.silabs.com/documents/public/application-notes/an0062.pdf
- http://infocenter.arm.com/help/topic/com.arm.doc.ddi0337e/DDI0337E_cortex_m3_r1p1_trm.pdf
