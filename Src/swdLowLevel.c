#include "swdLowLevel.h"
#include "main.h"
#include "stm32f1xx_hal_conf.h"
#include "stm32f1xx_hal.h"

void swdDelay(volatile uint32_t cycles)
{
  while(cycles--);
}

void swdWriteBit(int bit)
{
    HAL_GPIO_WritePin(SWDCK_GPIO_Port, SWDCK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SWDIO_GPIO_Port, SWDIO_Pin, bit);
    swdDelay(250);
    HAL_GPIO_WritePin(SWDCK_GPIO_Port, SWDCK_Pin, GPIO_PIN_SET);
    swdDelay(250);
}

int swdReadBit(void)
{
	int data;
	HAL_GPIO_WritePin(SWDCK_GPIO_Port, SWDCK_Pin, GPIO_PIN_SET);
	swdDelay(250);
	data = HAL_GPIO_ReadPin(SWDIO_GPIO_Port, SWDIO_Pin);
	HAL_GPIO_WritePin(SWDCK_GPIO_Port, SWDCK_Pin, GPIO_PIN_RESET);
	swdDelay(250);

	return data;
}

void swdReadTurnAround()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = SWDIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(SWDCK_GPIO_Port, SWDCK_Pin, GPIO_PIN_RESET);
	swdDelay(250);
}

void swdWriteTurnAround()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	HAL_GPIO_WritePin(SWDCK_GPIO_Port, SWDCK_Pin, GPIO_PIN_SET);
	swdDelay(250);
	HAL_GPIO_WritePin(SWDCK_GPIO_Port, SWDCK_Pin, GPIO_PIN_RESET);
	swdDelay(250);
	HAL_GPIO_WritePin(SWDCK_GPIO_Port, SWDCK_Pin, GPIO_PIN_SET);
	swdDelay(250);
	GPIO_InitStruct.Pin = SWDIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
