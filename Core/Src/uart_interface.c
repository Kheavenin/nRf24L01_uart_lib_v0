/*
 * uart_interface.c
 *
 *  Created on: Apr 18, 2020
 *      Author: Khevenin
 *
 *      Before use this small library init uart periphery in your STM32.
 *      Reference to HAL's uart structure passes as argument to all functions.
 */



/*	User include files	*/
#include "uart_interface.h"


/* Functions's bodies */

uint8_t sendBuffer(uint8_t *buffer, size_t size, UART_HandleTypeDef *huart) {
	if (size <= 0) {
		return 0;
	}
	uint8_t i;
	for (i = 0; i < size; i++) {
		sendShortInteger(buffer[i], huart);
	}
	return 1;
}

void sendString(const char *str, UART_HandleTypeDef *huart) {
	HAL_UART_Transmit_IT(huart, (uint8_t*) str, strlen(str));
}

void sendShortInteger(uint8_t number, UART_HandleTypeDef *huart) {
	char data[8];
	size_t dataSize = sprintf(data, "%hd", number);
	HAL_UART_Transmit_IT(huart, (uint8_t*) data, dataSize);
}

void sendInteger(uint16_t number, UART_HandleTypeDef *huart) {
	char data[8];
	size_t dataSize = sprintf(data, "%d", number);
	HAL_UART_Transmit_IT(huart, (uint8_t*) data, dataSize);
}

void sendLongInteger(uint32_t number, UART_HandleTypeDef *huart) {
	char data[8];
	size_t dataSize = sprintf(data, "%ld", number);
	HAL_UART_Transmit_IT(huart, (uint8_t*) data, dataSize);
}

/**
 * @brief Sending via uart float types variable.
 * @param float type variable
 * @param precision -  number of digits after point
 * @param pointer/references to HAL's uart structure
 */
void sendFloat(float number, uint8_t pointNumber, UART_HandleTypeDef *huart) {
	uint32_t afterPoint = 10;
	if (pointNumber > 6 || pointNumber <= 0) {
		afterPoint = 1000000;
	} else {
		int i;
		for (i = 0; i < pointNumber - 1; i++) {
			afterPoint *= 10;
		}
	}

	char *sign;
	if (number < 0)
		sign = "-";
	else
		sign = "";

	uint32_t tmpInt = number;
	float tmpFrac = number - tmpInt;
	uint32_t tmpInt2 = trunc(tmpFrac);
	tmpInt2 = trunc(tmpFrac * afterPoint);

	char data[16];
	size_t dataSize = sprintf(data, "%s%ld.%ld", sign, tmpInt, tmpInt2);
	HAL_UART_Transmit_IT(huart, (uint8_t*) data, dataSize);
}


