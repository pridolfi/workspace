/* Copyright 2015, Pablo Ridolfi
 *
 * This file is part of ESP8266_Driver.
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

/** @brief Main driver source file.
 **
 **/

/** \addtogroup esp8266_driver ESP8266 Driver
 ** @{ */

/*
 * Initials     Name
 * ---------------------------
 * PR           Pablo Ridolfi
 */

/*
 * modification history (new versions first)
 * -----------------------------------------------------------
 * 20150612 v0.0.1   PR first version
 */

/*==================[inclusions]=============================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ciaaUART.h"
#include "ciaaTick.h"

#include "esp8266_driver.h"

/*==================[macros and definitions]=================================*/

#define BUF_LEN 4096
#define TIMEOUT 100

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

static char buffer[BUF_LEN];

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

espStatus_e espCommand(char * cmd, char * rsp, int rsplen)
{
    espStatus_e rv = ESP_TIMEOUT;

    int i, pos = 0;
    int to = 0;

    rs232Print(cmd);

    for(i = 0; i < BUF_LEN; i++) {
    	buffer[i] = 0;
    }

    do {
        i = uartRecv(CIAA_UART_232, buffer+pos, BUF_LEN-pos);
        pos += i;

        to = (i == 0) ? to+1 : 0;

        if (memmem(buffer, BUF_LEN, "\r\nready\r\n", 9)) {
            rv = ESP_READY;
        }
        if (memmem(buffer, BUF_LEN, "\r\nOK\r\n", 6)) {
            rv = ESP_OK;
        }
        if (memmem(buffer, BUF_LEN, "\r\nERROR\r\n", 9)) {
            rv = ESP_ERROR;
        }
        if (memmem(buffer, BUF_LEN, "\r\nno change\r\n", 13)) {
            rv = ESP_NO_CHANGE;
        }

        pausems(100);

    } while ((to < TIMEOUT)&&(rv == ESP_TIMEOUT)&&(pos < BUF_LEN));

    if (pos >= BUF_LEN) {
        rv = ESP_BUF_FULL;
    }
    if ((to < TIMEOUT)&&(rsp != NULL)&&(rsplen > 0)) {
        strncpy(rsp, buffer, rsplen);
    }

    return rv;
}

espStatus_e espInit(espMode_e m)
{
    int rv;
    /* user must initialize RS232 UART!!! */
    rv = espCommand("AT+RST\r\n", 0, 0);
    if (rv == ESP_OK) {
        rv = espCommand("", 0, 0);
        if (rv == ESP_READY) {
            char str[100];
            sprintf(str, "AT+CWMODE=%u\r\n", m);
            rv = espCommand(str, 0, 0);
        }
    }
    return rv;
}

espStatus_e espListAP(char * rsp, int rsplen)
{
    return espCommand("AT+CWLAP\r\n", rsp, rsplen);
}

espStatus_e espConnectToAP(char * ssid, char * pw)
{
    char str[100];

    sprintf(str, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pw);

    return espCommand(str, 0, 0);
}

espStatus_e espStartUDP(char * url, uint16_t port, uint16_t localport)
{
    char str[100];

    sprintf(str, "AT+CIPSTART=\"UDP\",\"%s\",%u,%u,2\r\n", url, port, localport);

    return espCommand(str, 0, 0);
}

espStatus_e espGetIP(char * ip, int len)
{
    return espCommand("AT+CIFSR\r\n", ip, len);
}

int espGetData(void * data, int len)
{
    int rv = 0;
    int i;
    static int pos = 0;
    char * start;
    if (pos == 0) {
        bzero(buffer, BUF_LEN);
    }

    i = uartRecv(CIAA_UART_232, buffer+pos, BUF_LEN-pos);
    pos += i;

    start = memmem(buffer, pos, "\r\n+IPD,", 7);
    if (start) {
        int nbytes = atoi(start+7);

        memcpy(data, index(start+7, ':')+1, nbytes > len ? len : nbytes);

        rv = nbytes > len ? len : nbytes;

        start = 0;
        pos = 0;
    }
    return rv;
}

int32_t espSendData(void * data, int32_t len)
{
	char cmd[50];
	char rsp[50];

	sprintf(cmd, "AT+CIPSEND=%lu\r\n", len);

	uartSend(CIAA_UART_232, cmd, strlen(cmd));

	bzero(rsp, 50);

	while (NULL == memmem(rsp, 50, ">", 1)) {
		uartRecv(CIAA_UART_232, rsp, 50);
		pausems(1);
	}
	uartSend(CIAA_UART_232, data, len);
	return len;
}

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
