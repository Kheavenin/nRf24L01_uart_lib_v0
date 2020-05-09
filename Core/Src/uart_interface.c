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


const char nrfPowerUp[] = { "#nrf-pwr-up" }; //power up 		//size 12 with \0
const char nrfPowerDown[] = { "#nrf-pwr-dn" }; //power down		//size 12 with \0

const char nrfPowerTx0dBm[] = { "#nrf-pwr-tx-0" };			//size 14 with \0
const char nrfPowerTx6dBm[] = { "#nrf-pwr-tx-1" };			//size 14 with \0
const char nrfPowerTx12dBm[] = { "#nrf-pwr-tx-2" };			//size 14 with \0
const char nrfPowerTx18dBm[] = { "#nrf-pwr-tx-3" };			//size 14 with \0

const char nrfDataRate250kbps[] = { "#nrf-rate-0" };		//size 12 with \0
const char nrfDataRate1Mbps[] = { "#nrf-rate-1" };			//size 12 with \0
const char nrfDataRate2Mbps[] = { "#nrf-rate-2" };			//size 12 with \0

const char nrfChannel[] = { "#nrf-ch-" };//size 9 with \0 + 3 signs of channel

const char nrfReadRegister[] = { "#nrf-read-" }; //size 11 with \0 + 4  format: #nrf-read-0x01
const char nrfWriteRegister[] = { "#nrf-write-" }; //size 12 with \0	format: #nrf-write-0x01

const char nrfEnter[] = { "#nrf-enter" };		//size 11 with \0
const char nrfExit[] = { "#nrf-exit" };			//size 10 with \0
const char nrfPrompt[] = { "\n\r#nrf> " };

const char *nrfCommandTable[COMMAND_TABLE_SIZE] = { nrfPowerUp, nrfPowerDown,
		nrfPowerTx0dBm, nrfPowerTx6dBm, nrfPowerTx12dBm, nrfPowerTx18dBm,
		nrfDataRate250kbps, nrfDataRate1Mbps, nrfDataRate2Mbps, nrfChannel,
		nrfReadRegister, nrfWriteRegister };

/* Create struct */
nRF_UartStruct_t* nRF_UartInit(nrfStruct_t *nrfStruct,
		UART_HandleTypeDef *huart) {

	/* Creat struct */
	static nRF_UartStruct_t nrfUartStruct;
	static nRF_UartStruct_t *pnrfUartStruct = &nrfUartStruct;

	pnrfUartStruct->nrfStruct = nrfStruct;
	pnrfUartStruct->nrfUartStruct = huart;

	pnrfUartStruct->uartIrqFlag = 0;
	pnrfUartStruct->uartPromptFlag = 0;

	pnrfUartStruct->uartNrfChannel = 0;
	pnrfUartStruct->uartNrfReadReg = 0;
	pnrfUartStruct->uartNrfWriteReg = 0;
	pnrfUartStruct->uartNrfWriteVal = 0;

	resetChar(pnrfUartStruct->uartTxBuffer, UART_BUFFER_SIZE_TX);
	resetChar(pnrfUartStruct->uartRxBuffer, UART_BUFFER_SIZE_RX);
	resetChar(pnrfUartStruct->uartTemporaryBuffer, UART_BUFFER_SIZE_TMP);

	return pnrfUartStruct;
}

/* Enter to nRF mode */
uint8_t nrfModeEnter(nRF_UartStruct_t *nRF_UartStruct) {
	if (strstr(nRF_UartStruct->uartTemporaryBuffer, nrfEnter) != NULL) {
		nRF_UartStruct->uartPromptFlag = 1;
		/*
		sendString(nrfPrompt, &huart2);
		HAL_Delay(10);
		sendString("\n\rnRF24L01 access available\n", &huart2);
		 */
		return 1;
	}
	return 0;
}

/* Exit nRF mode */
uint8_t nrfModeExit(nRF_UartStruct_t *nRF_UartStruct) {
	if (strstr(nRF_UartStruct->uartTemporaryBuffer, nrfExit) != NULL) {
		nRF_UartStruct->uartPromptFlag = 0;
		/*
		sendString(nrfPrompt, &huart2);
		HAL_Delay(10);
		sendString("nRF24L01 access not available\n", &huart2);
		 */
		return 1;
	}
	return 0;
}

/* Detect and execute commands (others than Exit and Enter) */
uint8_t nrfModeCommand(nRF_UartStruct_t *nRF_UartStruct) {
	/* Find command */
	int8_t commandNumber = detectCommand(nRF_UartStruct,
			nRF_UartStruct->uartTemporaryBuffer);
	/* Execute command */
	if (commandNumber > 0) {
		executeCommand(nRF_UartStruct, commandNumber);
		return 1;
	}
	return -1;
}


/* Functions's bodies */
int8_t detectCommand(nRF_UartStruct_t *nRF_UartStruct, const char *str) {
	if (strlen(str) < MINIMUM_COMMAND_SIZE) //Check min size of command
		return -1;
	/* Check command */
	uint8_t i;
	for (i = 0; i < COMMAND_TABLE_SIZE; i++) {
		if (strstr(str, nrfCommandTable[i]) != NULL) {
			/* If it's change channel command read channel number */
			if (i == 9) {
				/* Wrong channel's number */
				if (detectChannel(nRF_UartStruct, str) == -1)
					return -1;
			}
			if (i == 10) {
				/* Wrong address to read */
				if (detectReadAddress(nRF_UartStruct, str) == -1)
					return -1;
			}
			return i;
		}
	}
	return -1;
}

int8_t detectReadAddress(nRF_UartStruct_t *nRF_UartStruct, const char *str) {
	if (strlen(str) <= 14) {
		return -1;
	}
	/* Find position of command in string */
	char chNum[4];
	strncpy(chNum, (strstr(str, nrfCommandTable[10]) + strlen(nrfCommandTable[10])), 5);
	uint8_t addr = atoi(chNum);

	/* Check address  */
	if (addr < 0x00 || addr > 0x1D) {
		return -1;
	}

	nRF_UartStruct->uartNrfReadReg = addr;
	return addr;
}

int8_t detectChannel(nRF_UartStruct_t *nRF_UartStruct, const char *str) {
	if (strlen(str) < 9) {
		return -1;
	}
	/* Find position of command in string */
	char chNum[4];
	/* strlen(nrfCommandTable[9])is offset of command  "#nrf-ch-" ,  necessary to find number of channel */
	strncpy(chNum, (strstr(str, nrfCommandTable[9]) + strlen(nrfCommandTable[9])), 3);
	int8_t channel = atoi(chNum);	//conversion string channel number to u_int

	/* Check channel number*/
	if (channel > 125 || channel < 0) {
		return -1;
	}
	/* Write channel number to structure */
	nRF_UartStruct->uartNrfChannel = channel;

	return channel;
}

int8_t executeCommand(nRF_UartStruct_t *nRF_UartStruct, uint8_t cmdNum) {
	switch (cmdNum) {
	case 0:
		/* Execute Power Up */
		pwrUp(nRF_UartStruct->nrfStruct);
		sendString("\n\rnRF24L01 module power up.", nRF_UartStruct->nrfUartStruct);
		HAL_Delay(5);
		return 0;
		break;
	case 1:
		/* Execute Power Down */
		pwrDown(nRF_UartStruct->nrfStruct);
		sendString("\n\rnRF24L01 module power down.", nRF_UartStruct->nrfUartStruct);
		HAL_Delay(5);
		return 1;
		break;
	case 2:
		/* Set power 0dBm */
		setRFpower(nRF_UartStruct->nrfStruct, RF_PWR_0dBm);
		sendString("\n\rSet RF power 0dBm.", nRF_UartStruct->nrfUartStruct);
		HAL_Delay(5);
		return 2;
		break;
	case 3:
		/* Set power -6dBm */
		setRFpower(nRF_UartStruct->nrfStruct, RF_PWR_6dBm);
		sendString("\n\rSet RF power -6dBm.", nRF_UartStruct->nrfUartStruct);
		HAL_Delay(5);
		return 3;
		break;
	case 4:
		/* Set power -12dBm */
		setRFpower(nRF_UartStruct->nrfStruct, RF_PWR_12dBm);
		sendString("\n\rSet RF power -12dBm.", nRF_UartStruct->nrfUartStruct);
		HAL_Delay(5);
		return 4;
		break;
	case 5:
		/* Set power -18dBm */
		setRFpower(nRF_UartStruct->nrfStruct, RF_PWR_18dBm);
		sendString("\n\rSet RF power -18dBm.", nRF_UartStruct->nrfUartStruct);
		HAL_Delay(5);
		return 5;
		break;
	case 6:
		/* Set 250kbps data rate */
		setDataRate(nRF_UartStruct->nrfStruct, RF_DataRate_250);
		sendString("\n\rSet data rate 250kBps.", nRF_UartStruct->nrfUartStruct);
		HAL_Delay(5);
		return 6;
		break;
	case 7:
		/* Set 1Mbps data rate */
		setDataRate(nRF_UartStruct->nrfStruct, RF_DataRate_1M);
		sendString("\n\rSet data rate 1Mbps.", nRF_UartStruct->nrfUartStruct);
		HAL_Delay(5);
		return 7;
		break;
	case 8:
		/* Set 2Mbps data rate */
		setDataRate(nRF_UartStruct->nrfStruct, RF_DataRate_2M);
		sendString("\n\rSet data rate 2Mbps.", nRF_UartStruct->nrfUartStruct);
		HAL_Delay(5);
		return 8;
		break;
	case 9:
		/* Set channel */
		setChannel(nRF_UartStruct->nrfStruct, nRF_UartStruct->uartNrfChannel);
		sendString("\n\rSet RF channel.", nRF_UartStruct->nrfUartStruct);	//log
		HAL_Delay(5);
		return 9;
		break;
	case 10:
		/* read nRF register */
		sendString("\n\rRead nRF register.", nRF_UartStruct->nrfUartStruct);	//log
		HAL_Delay(5);
		/* In this line execute read nRF's register */
		return 10;
		break;
	case 11:
		/* Write nRF register */
		sendString("\n\rWrite nRF register.", nRF_UartStruct->nrfUartStruct);	//log
		HAL_Delay(5);
		/* In this line execute write to nRF's register */
		return 11;
		break;
	default:
		sendString("\n\rInvalid command number.", nRF_UartStruct->nrfUartStruct);
		HAL_Delay(50);
		return -1;
		break;
	}
	/* Switch end */
	return -1;
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
