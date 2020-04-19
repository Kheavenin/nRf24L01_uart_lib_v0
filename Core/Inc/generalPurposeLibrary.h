/*
 * generalPurposeLibrary.h
 *
 *  Created on: Apr 18, 2020
 *      Author: Khevenin
 */

#ifndef INC_GENERALPURPOSELIBRARY_H_
#define INC_GENERALPURPOSELIBRARY_H_

#include "stddef.h"


void resetTable(float *tab, size_t tabSize);

float addTable(float *tab, size_t tabSize);
float calcMeanVal(float *tab, size_t tabSize);


#endif /* INC_GENERALPURPOSELIBRARY_H_ */
