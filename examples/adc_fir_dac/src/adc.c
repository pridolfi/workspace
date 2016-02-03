/*
 * adc.c
 *
 *  Created on: Sep 4, 2013
 *      Author: Pablo
 */

#include "board.h"
#include "fir_q31.h"
#include "main.h"

int adcFlag=0;

#ifdef lpc4337_m4
#define LPC_ADC LPC_ADC0
#define ADC_IRQn ADC0_IRQn
#endif

/* P0.23 -> AD0 */
void adcInit(void)
{
	ADC_CLOCK_SETUP_T adc;

	Chip_ADC_Init(LPC_ADC, &adc);
	Chip_ADC_SetSampleRate(LPC_ADC, &adc, 22000);

	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);
	Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH0, ENABLE);
	Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);

	NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_IRQHandler(void)
{
	static uint16_t data;
	Chip_ADC_ReadValue(LPC_ADC, ADC_CH0, &data);
#if(USAR_FUNCIONES_ASSEMBLER)
	asm_fir_q31_put(&filtro, data>>2);
#else
	fir_q31_put(&filtro, data>>2);
#endif
	adcFlag=1;
}
