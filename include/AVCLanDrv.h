#ifndef AVCLANDRV_H_
#define AVCLANDRV_H_

#include "cmsis_device.h"
extern "C" {
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
}
#include "timer.h"
#include "uart.h"
#include "util.h"

#include "AVCLanMsg.h"

class AVCLanDrvBase {
	protected:
		p_timer timer;

		// Times in timer ticks
		const uint32_t T_StartBit	= timer.uS(	170	);
		const uint32_t T_Bit		= timer.uS(	39	);
		const uint32_t T_Bit_1		= 		T_Bit 	/ 2;
		const uint32_t T_Bit_0		= (4 *	T_Bit) 	/ 5;
		const uint32_t T_BitMeasure	= (T_Bit_1 + T_Bit_0) / 2;
		const uint32_t T_EndWait	= timer.uS(	100	);
		const uint32_t T_Timeout	= timer.uS( 200 );

		// ------------------------

		PinCfgType AVC_RX_PIN = {
				Portnum: 	0,
				Pinnum:		4,
				// Timer Capture function
				Funcnum:	PIN_FUNC_3,
				Pinmode:	PIN_PINMODE_PULLUP,
				OpenDrain:	PIN_PINMODE_NORMAL,
				GpioDir:	GPIO_DIR_INPUT
		};
		PinCfgType AVC_TX_PIN = {
				Portnum: 	0,
				Pinnum:		5,
				Funcnum:	PIN_FUNC_0,
				Pinmode:	PIN_PINMODE_TRISTATE,
				OpenDrain:	PIN_PINMODE_OPENDRAIN,
				GpioDir:	GPIO_DIR_INPUT
		};
		PinCfgType AVC_STB_PIN = {
				Portnum: 	2,
				Pinnum:		5,
				Funcnum:	PIN_FUNC_0,
				Pinmode:	PIN_PINMODE_TRISTATE,
				OpenDrain:	PIN_PINMODE_OPENDRAIN,
				GpioDir:	GPIO_DIR_OUTPUT
		};
		PinCfgType AVC_EN_PIN = {
				Portnum: 	1,
				Pinnum:		17,
				Funcnum:	PIN_FUNC_0,
				Pinmode:	PIN_PINMODE_TRISTATE,
				OpenDrain:	PIN_PINMODE_NORMAL,
				GpioDir:	GPIO_DIR_OUTPUT
		};
		PinCfgType AVC_ERR_PIN = {
				Portnum: 	2,
				Pinnum:		7,
				Funcnum:	PIN_FUNC_0,
				Pinmode:	PIN_PINMODE_TRISTATE,
				OpenDrain:	PIN_PINMODE_NORMAL,
				GpioDir:	GPIO_DIR_OUTPUT
		};

		void setStandby(bool isOn)	{ gpioPinWrite(AVC_STB_PIN, isOn); }
		// Active low
		void setEnabled(bool isOn)	{ gpioPinWrite(AVC_EN_PIN, !isOn); }

	public:
		AVCLanDrvBase(p_timer);
};

#endif
