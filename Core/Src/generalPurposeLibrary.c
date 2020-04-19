/*
 * generalPurposeLibrary.c
 *
 *  Created on: Apr 18, 2020
 *      Author: Khevenin
 */

#include "generalPurposeLibrary.h"

void resetTable(float *tab, size_t tabSize) {
	size_t i;
	for (i = 0; i < tabSize; i++) {
		tab[i] = 0;
	}
}

float addTable(float *tab, size_t tabSize) {
	float sum = 0;
	size_t i;
	for (i = 0; i < tabSize; i++) {
		sum += tab[i];
	}
	return sum;
}

float calcMeanVal(float *tab, size_t tabSize) {
	return (addTable(tab, tabSize) / tabSize);
}
