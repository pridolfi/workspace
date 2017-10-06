/* Copyright 2016, Pablo Ridolfi
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "ciaaIO.h"

#define IN_COUNT 2
#define OUT_COUNT 4
ciaaPin_t inputs[IN_COUNT] = { {0,18},{0,1} };
ciaaPin_t outputs[OUT_COUNT] = { {2,2},{2,3},{2,4},{0,22} };

void ciaaIOInit(void)
{
	Chip_GPIO_Init(LPC_GPIO);
	Chip_IOCON_Init(LPC_IOCON);
	for (int i=0; i<IN_COUNT; i++) {
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, inputs[i].port, inputs[i].bit);
		Chip_IOCON_PinMux(LPC_IOCON, inputs[i].port, inputs[i].bit, IOCON_MODE_PULLUP, IOCON_FUNC0);
	}
	for (int i=0; i<OUT_COUNT; i++) {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, inputs[i].port, inputs[i].bit);
		Chip_GPIO_SetPinOutLow(LPC_GPIO, inputs[i].port, inputs[i].bit);
		Chip_IOCON_PinMux(LPC_IOCON, inputs[i].port, inputs[i].bit, IOCON_MODE_PULLUP, IOCON_FUNC0);
	}
}

uint32_t ciaaReadInput(uint32_t inputNumber)
{
	return Chip_GPIO_ReadPortBit(LPC_GPIO,
			inputs[inputNumber].port,
			inputs[inputNumber].bit);
}

uint32_t ciaaReadOutput(uint32_t outputNumber)
{
	return Chip_GPIO_ReadPortBit(LPC_GPIO,
			outputs[outputNumber].port,
			outputs[outputNumber].bit);
}

uint32_t ciaaWriteOutput(uint32_t outputNumber, uint32_t value)
{
	Chip_GPIO_SetPinState(LPC_GPIO,
			outputs[outputNumber].port,
			outputs[outputNumber].bit,
			value);
	return 0;
}

void ciaaToggleOutput(uint32_t outputNumber)
{
	Chip_GPIO_SetPinToggle(LPC_GPIO,
			outputs[outputNumber].port,
			outputs[outputNumber].bit);
}
