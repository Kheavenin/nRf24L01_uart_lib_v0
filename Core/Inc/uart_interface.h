/*
 * uart_interface.h
 *
 *  Created on: Apr 18, 2020
 *      Author: Khevenin
 *
 *      Before use this small library init uart periphery in your STM32.
 *      Reference to HAL's uart structure passes as argument to all functions.
 */

#ifndef INC_UART_INTERFACE_H_
#define INC_UART_INTERFACE_H_

/* Standard include file */
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "nRF24L01.h"

/* Define */

#define UART_BUFFER_SIZE_TX 32
#define UART_BUFFER_SIZE_RX 32
#define UART_BUFFER_SIZE_TMP 32

#define MINIMUM_COMMAND_SIZE 9 //8
#define COMMAND_TABLE_SIZE 10

/* Extern */
extern UART_HandleTypeDef huart2;

/* Const */
extern const char nrfPowerUp[];
extern const char nrfPowerDown[];

extern const char nrfPowerTx0dBm[];
extern const char nrfPowerTx6dBm[];
extern const char nrfPowerTx12dBm[];
extern const char nrfPowerTx18dBm[];

extern const char nrfDataRate250kbps[];
extern const char nrfDataRate1Mbps[];
extern const char nrfDataRate2Mbps[];

extern const char nrfChannel[];
extern const char *nrfCommandTable[COMMAND_TABLE_SIZE];

extern const char nrfEnter[];
extern const char nrfExit[];
extern const char nrfPrompt[];

typedef struct {
	nrfStruct_t *nrfStruct;
	UART_HandleTypeDef *nrfUartStruct;

	uint8_t uartIrqFlag;
	uint8_t uartPromptFlag;
	uint8_t uartNrfChannel;
	char uartTxBuffer[UART_BUFFER_SIZE_TX];
	char uartRxBuffer[UART_BUFFER_SIZE_RX];
	char uartTemporaryBuffer[UART_BUFFER_SIZE_TMP];

} nRF_UartStruct_t;


nRF_UartStruct_t* nRF_UartInit(nrfStruct_t *nrfStruct,
		UART_HandleTypeDef *huart);

uint8_t nrfModeEnter(nRF_UartStruct_t *nRF_UartStruct);
uint8_t nrfModeExit(nRF_UartStruct_t *nRF_UartStruct);

/* Functions's prototypes */
int8_t executeCommand(nrfStruct_t *nrfStruct, uint8_t cmdNum,
		const char *str);
int8_t detectCommand(nRF_UartStruct_t *nRF_UartStruct, const char *str);
int8_t channelDetect(nRF_UartStruct_t *nRF_UartStruct, const char *str);

uint8_t sendBuffer(uint8_t *buffer, size_t size, UART_HandleTypeDef *huart);

void sendString(const char *str, UART_HandleTypeDef *huart);
void sendChar(char charakter, UART_HandleTypeDef *huart);

void sendShortInteger(uint8_t number, UART_HandleTypeDef *huart);
void sendInteger(uint16_t number, UART_HandleTypeDef *huart);
void sendLongInteger(uint32_t number, UART_HandleTypeDef *huart);
void sendFloat(float number, uint8_t pointNumber, UART_HandleTypeDef *huart);

uint8_t resetChar(char *buf, size_t bufSize);

#endif /* INC_UART_INTERFACE_H_ */
