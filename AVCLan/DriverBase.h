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

#include "IEBusMessage.h"

class DriverBase {
	/*
	 * The DriverBase class contains things used by both Rx and Tx,
	 * such as the timing information of bits,
	 * and the GPIO pins used for RX and TX.
	 */
	public:
		typedef uint32_t Time;

	protected:
		p_timer timer;

		static constexpr float T_Bit_uS = 256. / 6.291456;

		// Times in timer ticks
		const uint32_t T_StartBit	= timer.uS(	170	);
		const uint32_t T_Bit		= timer.uS(	T_Bit_uS );
		const uint32_t T_Bit_1		= timer.uS(		T_Bit_uS / 2. );
		const uint32_t T_Bit_0		= timer.uS(	4.* T_Bit_uS / 5. );
		const uint32_t T_BitMeasure	= (T_Bit_1 + T_Bit_0) / 2;
		const uint32_t T_TxWait		= timer.uS(	600	);
		const uint32_t T_Timeout	= timer.uS( 2000 );

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
				GpioDir:	GPIO_DIR_OUTPUT
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

		bool getRxPinState();
		void setTxPinState(bool isOn);

		void setStandby(bool isOn);
		void setEnabled(bool isOn);

	public:
		DriverBase(p_timer);
};

#endif
