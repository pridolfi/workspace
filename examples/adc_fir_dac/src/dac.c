/*
 * dac.c
 *
 *  Created on: Sep 4, 2013
 *      Author: Pablo
 */


#include "board.h"

/* P0.26 -> AOUT */
void dacInit(void)
{
	Chip_DAC_Init(LPC_DAC);
}

void dacWrite(uint32_t v)
{
	Chip_DAC_UpdateValue(LPC_DAC, v);
}
