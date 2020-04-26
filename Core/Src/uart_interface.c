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

const char nrfCommandPreamble[PREMBLE_TABLE_SIZE] = { '#', 'n', 'r', 'f', '-' };

const char nrfPowerUp[] = { 'p', 'w', 'r', '-', 'u', 'p' }; //power up
const char nrfPowerDown[] = { 'p', 'w', 'r', '-', 'd', 'n' }; //power down

const char nrfPowerTx0dBm[] = { 'p', 'w', 'r', '-', 't', 'x', '-', '0' };
const char nrfPowerTx6dBm[] = { 'p', 'w', 'r', '-', 't', 'x', '-', '1' };
const char nrfPowerTx12dBm[] = { 'p', 'w', 'r', '-', 't', 'x', '-', '2' };
const char nrfPowerTx18dBm[] = { 'p', 'w', 'r', '-', 't', 'x', '-', '3' };

const char nrfDataRate250kbps[] = { 'r', 'a', 't', 'e', '-', '0' };
const char nrfDataRate1Mbps[] = { 'r', 'a', 't', 'e', '-', '1' };
const char nrfDataRate2Mbps[] = { 'r', 'a', 't', 'e', '-', '2' };

const char nrfChannel[] = { 'c', 'h', 'a', 'n', 'n', 'e', 'l', '-' };

const char *nrfCommandTable[COMMAND_TABLE_SIZE] = { nrfPowerUp, nrfPowerDown,
		nrfPowerTx0dBm, nrfPowerTx6dBm, nrfPowerTx12dBm, nrfPowerTx18dBm,
		nrfDataRate250kbps,
		nrfDataRate1Mbps, nrfDataRate2Mbps, nrfChannel };

/* Functions's bodies */
uint8_t detectCommand(const char *str, const char **cmdTab, size_t strLen) {
	if (strLen < MINIMUM_COMMAND_SIZE) //Check min size of command
		return -1;
	/* First command */
	uint8_t i;
	char *p = NULL;
	for (i = 0; i < COMMAND_TABLE_SIZE; i++) {
		p = strstr(str, cmdTab[i]);
		if (p != NULL) {
			return i;
		}
	}
	return -1;
}

uint8_t executeCommand(nrfStruct_t *nrfStruct, uint8_t commandNumber) {
	switch (commandNumber) {
	case 0:
		/* Execute Power Up */
//		pwrUp(nrfStruct);
		sendString("\n\rExecuted 1st command.", &huart2);
		HAL_Delay(50);
		return 1;
		break;
	case 1:
//		pwrDown(nrfStruct);
		sendString("\n\rExecuted 2nd command.", &huart2);
		HAL_Delay(50);
		return 1;
		break;
	case 2:
		sendString("\n\rExecuted 3rd command.", &huart2);
		HAL_Delay(50);
		return 1;
		break;
	case 3:
		sendString("\n\rExecuted 4th command.", &huart2);
		HAL_Delay(50);
		return 1;
		break;
	case 4:
		sendString("\n\rExecuted 5th command.", &huart2);
		HAL_Delay(50);
		return 1;
		break;
	case 5:
		sendString("\n\rExecuted 6th command.", &huart2);
		HAL_Delay(50);
		return 1;
		break;
	case 6:
		sendString("\n\rExecuted 7th command.", &huart2);
		HAL_Delay(50);
		return 1;
		break;
	case 7:
		sendString("\n\rExecuted 8th command.", &huart2);
		HAL_Delay(50);
		return 1;
		break;
	case 8:
		sendString("\n\rExecuted 9th command.", &huart2);
		HAL_Delay(50);
		return 1;
		break;
	case 9:
		sendString("\n\rExecuted 10th command.", &huart2);
		HAL_Delay(50);
		return 1;
		break;
	default:
		sendString("\n\rInvalid command.", &huart2);
		HAL_Delay(50);
		return 0;
		break;
	}

	return 0;
}


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

void sendChar(char charakter, UART_HandleTypeDef *huart) {
	char data[4];
	size_t dataSize = sprintf(data, "%cb", charakter);
	HAL_UART_Transmit_IT(huart, (uint8_t*) data, dataSize);
}

void sendShortInteger(uint8_t number, UART_HandleTypeDef *huart) {
	char data[4];
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

uint8_t resetChar(char *buf, size_t bufSize) {
	if (bufSize <= 0 || bufSize > 256)
		return 0;
	else {
		uint8_t i;
		for (i = 0; i < (bufSize - 1); i++) {
			buf[i] = 0x00;
		}
		return 1;
	}
}
