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
