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
#include <string.h>
#include <math.h>


/*	User include files	*/

/* Extern */
const char nrfCommandPreamble[] = { '#', 'n', 'r', 'f', '-' };

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

const char *nrfCommandTable[] = { nrfPowerUp, nrfPowerDown, nrfPowerTx0dBm,
		nrfPowerTx6dBm, nrfPowerTx12dBm, nrfPowerTx18dBm, nrfDataRate250kbps,
		nrfDataRate1Mbps, nrfDataRate2Mbps, nrfChannel };

/* Functions's prototypes */

uint8_t detectCommand(const char *str, const char **cmdTab, size_t strLen,
		size_t cmdLen);

uint8_t sendBuffer(uint8_t *buffer, size_t size, UART_HandleTypeDef *huart);

void sendString(const char *str, UART_HandleTypeDef *huart);
void sendChar(char charakter, UART_HandleTypeDef *huart);

void sendShortInteger(uint8_t number, UART_HandleTypeDef *huart);
void sendInteger(uint16_t number, UART_HandleTypeDef *huart);
void sendLongInteger(uint32_t number, UART_HandleTypeDef *huart);
void sendFloat(float number, uint8_t pointNumber, UART_HandleTypeDef *huart);

#endif /* INC_UART_INTERFACE_H_ */
