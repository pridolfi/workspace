/*
 * @brief Virtual communication port example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "main.h"

#include "board.h"
#include <stdio.h>
#include <string.h>
#include "app_usbd_cfg.h"
#include "cdc_vcom.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#if defined(lpc4337_m4)
#define LPC_USB_BASE   LPC_USB0_BASE
#define USB_IRQn       USB0_IRQn
#define USB_IRQHandler USB0_IRQHandler
#endif

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

static USBD_HANDLE_T g_hUsb;
static uint8_t g_rxBuff[256];

#if defined(lpc1769)
extern const  USBD_HW_API_T hw_api;
extern const  USBD_CORE_API_T core_api;
extern const  USBD_CDC_API_T cdc_api;
/* Since this example only uses CDC class link functions for that clas only */
static const  USBD_API_T g_usbApi = {
	&hw_api,
	&core_api,
	0,
	0,
	0,
	&cdc_api,
	0,
	0x02221101,
};

const  USBD_API_T *g_pUsbApi = &g_usbApi;
#elif defined(lpc4337_m4)
const USBD_API_T *g_pUsbApi;
static uint32_t g_ep0RxBusy = 0;/* flag indicating whether EP0 OUT/RX buffer is busy. */
static USB_EP_HANDLER_T g_Ep0BaseHdlr;	/* variable to store the pointer to base EP0 handler */
#endif

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Initialize pin and clocks for USB0/USB1 port */
static void usb_pin_clk_init(void)
{
#if defined(lpc1769)
	/* enable USB PLL and clocks */
	Chip_USB_Init();
	/* enable USB 1 port on the board */
	Board_USBD_Init(1);
#elif defined(lpc4337)
	/* enable USB PLL and clocks */
	Chip_USB0_Init();
#endif
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/
#if defined(lpc4337_m4)
/* EP0_patch part of WORKAROUND for artf45032. */
ErrorCode_t EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	switch (event) {
	case USB_EVT_OUT_NAK:
		if (g_ep0RxBusy) {
			/* we already queued the buffer so ignore this NAK event. */
			return LPC_OK;
		}
		else {
			/* Mark EP0_RX buffer as busy and allow base handler to queue the buffer. */
			g_ep0RxBusy = 1;
		}
		break;

	case USB_EVT_SETUP:	/* reset the flag when new setup sequence starts */
	case USB_EVT_OUT:
		/* we received the packet so clear the flag. */
		g_ep0RxBusy = 0;
		break;
	}
	return g_Ep0BaseHdlr(hUsb, data, event);
}
#endif
/**
 * @brief	Handle interrupt from USB0
 * @return	Nothing
 */
void USB_IRQHandler(void)
{
	USBD_API->hw->ISR(g_hUsb);
}

/* Find the address of interface descriptor for given class type. */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass)
{
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength) {
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass) {
				break;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

/**
 * @brief	main routine for example
 * @return	Function should not exit.
 */
void cdcTask(void * p)
{
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	uint32_t prompt = 0, rdCnt = 0;

	/* enable clocks and pinmux */
	usb_pin_clk_init();

#if defined(lpc4337_m4)
	/* Init USB API structure */
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;
#endif

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
#if defined(lpc1769)
	usb_param.usb_reg_base = LPC_USB_BASE + 0x200;
	usb_param.max_num_ep = 3;
#elif defined(lpc4337_m4)
	usb_param.usb_reg_base = LPC_USB_BASE;
	usb_param.max_num_ep = 4;
#endif
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) &USB_DeviceDescriptor[0];
	desc.string_desc = (uint8_t *) &USB_StringDescriptor[0];
	/* Note, to pass USBCV test full-speed only devices should have both
	   descriptor arrays point to same location and device_qualifier set to 0.
	 */
	desc.high_speed_desc = (uint8_t *) &USB_FsConfigDescriptor[0];
	desc.full_speed_desc = (uint8_t *) &USB_FsConfigDescriptor[0];
#if defined(lpc1769)
	desc.device_qualifier = 0;
#else
	desc.device_qualifier = (uint8_t *) &USB_DeviceQualifier[0];
#endif

	/* USB Initialization */
	ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);
	if (ret == LPC_OK) {
#if defined(lpc4337_m4)
		/*	WORKAROUND for artf45032 ROM driver BUG:
		    Due to a race condition there is the chance that a second NAK event will
		    occur before the default endpoint0 handler has completed its preparation
		    of the DMA engine for the first NAK event. This can cause certain fields
		    in the DMA descriptors to be in an invalid state when the USB controller
		    reads them, thereby causing a hang.
		 */
		USB_CORE_CTRL_T *pCtrl;

		pCtrl = (USB_CORE_CTRL_T *) g_hUsb;	/* convert the handle to control structure */
		g_Ep0BaseHdlr = pCtrl->ep_event_hdlr[0];/* retrieve the default EP0_OUT handler */
		pCtrl->ep_event_hdlr[0] = EP0_patch;/* set our patch routine as EP0_OUT handler */
#endif
		/* Init VCOM interface */
		ret = vcom_init(g_hUsb, &desc, &usb_param);
		if (ret == LPC_OK) {
			/*  enable USB interrupts */
			NVIC_EnableIRQ(USB_IRQn);
			/* now connect */
			USBD_API->hw->Connect(g_hUsb, 1);
		}
	}

	DEBUGSTR("USB CDC class based virtual Comm port example!\r\n");

	while (1) {
		/* Check if host has connected and opened the VCOM port */
		if ((vcom_connected() != 0) && (prompt == 0)) {
			vcom_write((uint8_t *)"Hello World!!\r\n", 15);
			prompt = 1;
		}
		/* If VCOM port is opened echo whatever we receive back to host. */
		if (prompt) {
			rdCnt = vcom_bread(&g_rxBuff[0], 256);
			if (rdCnt) {
				Board_LED_Toggle(0);
				vcom_write(&g_rxBuff[0], rdCnt);

				/* Agregado: Envío por la queue los caracteres recibidos
				 * hacia la tarea que los escribe en el LCD
				 */
				uint32_t i;
				for(i=0; i<rdCnt; i++) {
					xQueueSend(qLCD, &g_rxBuff[i], portMAX_DELAY);
				}

			}
			/* Agregado: Si el semáforo fue liberado desde la ISR, envío
			 * un mensaje por la UART CDC. Take no bloqueante.
			 */
			if (xSemaphoreTake(sem, 0) == pdTRUE) {
				vcom_write((uint8_t *)"\r\n*** Boton pulsado ***\r\n", 25);
			}
		}
		/* Ceder CPU a la próxima tarea, evita inanición */
		portYIELD();
	}
}
