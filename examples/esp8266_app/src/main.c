/* Copyright 2015, Pablo Ridolfi
 *
 * This file is part of CIAA-IoT.
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

/** @brief CIAA-IoT main source file.
 **
 **/

/** \addtogroup CIAA-IoT-main CIAA-IoT main source files.
 ** @{ */

/*
 * Initials     Name
 * ---------------------------
 * PR			Pablo Ridolfi
 */

/*
 * modification history (new versions first)
 * -----------------------------------------------------------
 * 20150611 v0.0.1	PR first version
 */

/*==================[inclusions]=============================================*/

#include "main.h"

#include <stdio.h>
#include <string.h>

#include "ciaaIO.h"
#include "ciaaUART.h"
#include "ciaaTick.h"

#include "esp8266_driver.h"

/*==================[macros and definitions]=================================*/

#define BUF_LEN 4096

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

static char buffer[BUF_LEN];

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void setupHardware(void)
{
    SystemCoreClockUpdate();

    Board_Init();

    ciaaTickInit();
    ciaaIOInit();
    ciaaUARTInit();
}

/*==================[external functions definition]==========================*/

int main(void)
{
    espStatus_e s;

    setupHardware();

	printf("\r\n******************************");
	printf("\r\nCIAA-IoT v0.1 for EDU-CIAA-NXP");
	printf("\r\n******************************");
	printf("\r\n");

	ciaaWriteOutput(LEDR, 1);

	do {
	    s = espInit(ESP_STATION);
	    pausems(100);
	} while ((s != ESP_NO_CHANGE)&&(s != ESP_OK));

    if ((s == ESP_NO_CHANGE)||(s == ESP_OK)) {
        printf("ESP Ready!\r\n");
        ciaaWriteOutput(LEDG, 1);

        printf("Connecting...\r\n");

        s = espConnectToAP("PabloN5", "linuxrulez!");

        if (s == ESP_OK) {
            bzero(buffer, BUF_LEN);
            s = espGetIP(buffer, BUF_LEN);
            if (s == ESP_OK) {
                printf("Done! IP: %s", buffer);
                s = espStartUDP("192.168.43.40", 8000, 8001);
                if (s == ESP_OK) {
                    printf("Sending to port 8000\r\n");
                    ciaaWriteOutput(LEDR, 0);
                } else {
                    printf("Error %d :(\r\n", s);
                }
            }
        } else {
            printf("Error %d :(\r\n", s);
        }
    } else {
        printf("Error %d :(\r\n", s);
    }

    int rv;

	while (1) {
		pausems(500);
		bzero(buffer, BUF_LEN);
		rv = espGetData(buffer, BUF_LEN);
		if (rv > 0) {
			buffer[rv] = 0;
		    printf("Se recibieron %u bytes: %s.\r\n", rv, buffer);
		    espSendData(buffer, rv);
		    pausems(100);
		}
		espSendData("Hola!\n", 6);
	}
}

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
