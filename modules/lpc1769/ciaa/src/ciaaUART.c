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

#include "ciaaUART.h"

uint8_t rxbuf[2][UART_BUF_SIZE];
uint8_t txbuf[2][UART_BUF_SIZE];

RINGBUFF_T rrb[2];
RINGBUFF_T trb[2];

uartData_t uarts[2] =
{
		{ LPC_UART0, &(rrb[0]), &(trb[0]) },
		{ LPC_UART3, &(rrb[1]), &(trb[1]) }
};

void ciaaUARTInit(void)
{
	/* UART0 (USB/Debug) */
	Chip_UART_Init(LPC_UART0);
	Chip_UART_SetBaud(LPC_UART0, 115200);

	Chip_UART_TXEnable(LPC_UART0);

	Chip_IOCON_PinMux(LPC_IOCON, 0, 2, IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 3, IOCON_MODE_INACT, IOCON_FUNC1);

	/* Restart FIFOS: set Enable, Reset content, set trigger level */
	Chip_UART_SetupFIFOS(LPC_UART0, UART_FCR_FIFO_EN | UART_FCR_TX_RS | UART_FCR_RX_RS | UART_FCR_TRG_LEV0);
	/* dummy read */
	Chip_UART_ReadByte(LPC_UART0);
	/* enable rx interrupt */
	Chip_UART_IntEnable(LPC_UART0, UART_IER_RBRINT);

	NVIC_EnableIRQ(UART0_IRQn);

	RingBuffer_Init(uarts[0].rrb, rxbuf[0], 1, UART_BUF_SIZE);
	RingBuffer_Init(uarts[0].trb, txbuf[0], 1, UART_BUF_SIZE);

	/* UART3 (RS232) */
	Chip_UART_Init(LPC_UART3);
	Chip_UART_SetBaud(LPC_UART3, 115200);

	Chip_UART_TXEnable(LPC_UART3);

	Chip_IOCON_PinMux(LPC_IOCON, 0, 0, IOCON_MODE_INACT, IOCON_FUNC2);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 1, IOCON_MODE_INACT, IOCON_FUNC2);

	Chip_UART_IntEnable(LPC_UART3, UART_IER_RBRINT);
	NVIC_EnableIRQ(UART3_IRQn);

	RingBuffer_Init(uarts[1].rrb, rxbuf[1], 1, UART_BUF_SIZE);
	RingBuffer_Init(uarts[1].trb, txbuf[1], 1, UART_BUF_SIZE);
}

void uart_irq(ciaaUART_e n)
{
	uartData_t * u = &(uarts[n]);

	Chip_UART_IRQRBHandler(u->uart, u->rrb, u->trb);
}

void UART0_IRQHandler(void)
{
	uart_irq(CIAA_UART_USB);
}

void UART3_IRQHandler(void)
{
	uart_irq(CIAA_UART_232);
}

int uartSend(ciaaUART_e nUART, void * data, int datalen)
{
	uartData_t * u = &(uarts[nUART]);

	return Chip_UART_SendRB(u->uart, u->trb, data, datalen);
}

int uartRecv(ciaaUART_e nUART, void * data, int datalen)
{
	uartData_t * u = &(uarts[nUART]);

	return Chip_UART_ReadRB(u->uart, u->rrb, data, datalen);
}
