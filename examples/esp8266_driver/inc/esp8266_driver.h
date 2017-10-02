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

#ifndef _ESP8266_DRIVER_H_
#define _ESP8266_DRIVER_H_

/** @brief Main driver header file
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

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

typedef enum
{
    ESP_BUF_FULL    = -3,
    ESP_TIMEOUT     = -2,
    ESP_ERROR       = -1,
    ESP_OK          =  0,
    ESP_NO_CHANGE   =  1,
    ESP_READY       =  2
}espStatus_e;

typedef enum
{
    ESP_STATION     = 1,
    ESP_AP          = 2,
    ESP_AP_STATION  = 3
}espMode_e;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

espStatus_e espInit(espMode_e m);
espStatus_e espListAP(char * rsp, int rsplen);
espStatus_e espConnectToAP(char * ssid, char * pw);
espStatus_e espStartUDP(char * url, uint16_t port, uint16_t localport);
espStatus_e espGetIP(char * ip, int len);
int espGetData(void * data, int len);
espStatus_e espCommand(char * cmd, char * rsp, int rsplen);
int32_t espSendData(void * data, int32_t len);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* _ESP8266_DRIVER_H_ */
