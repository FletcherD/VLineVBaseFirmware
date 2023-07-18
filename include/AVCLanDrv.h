#include "cmsis_device.h"
extern "C" {
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "LPC17xx.h"
}
#include "timer.h"
#include "uart.h"
#include "util.h"

class AVCLanDrv{
	private:
		p_timer timer;

		PINSEL_CFG_Type AVC_RX_PIN = {
				Portnum: 	0,
				Pinnum:		4,
				Funcnum:	PINSEL_FUNC_0,
				Pinmode:	PINSEL_PINMODE_TRISTATE,
				OpenDrain:	PINSEL_PINMODE_NORMAL
		};
		PINSEL_CFG_Type AVC_TX_PIN = {
				Portnum: 	0,
				Pinnum:		5,
				Funcnum:	PINSEL_FUNC_0,
				Pinmode:	PINSEL_PINMODE_TRISTATE,
				OpenDrain:	PINSEL_PINMODE_OPENDRAIN
		};
		PINSEL_CFG_Type AVC_STB_PIN = {
				Portnum: 	2,
				Pinnum:		5,
				Funcnum:	PINSEL_FUNC_0,
				Pinmode:	PINSEL_PINMODE_TRISTATE,
				OpenDrain:	PINSEL_PINMODE_OPENDRAIN
		};
		PINSEL_CFG_Type AVC_EN_PIN = {
				Portnum: 	2,
				Pinnum:		6,
				Funcnum:	PINSEL_FUNC_0,
				Pinmode:	PINSEL_PINMODE_TRISTATE,
				OpenDrain:	PINSEL_PINMODE_NORMAL
		};
		PINSEL_CFG_Type AVC_ERR_PIN = {
				Portnum: 	2,
				Pinnum:		7,
				Funcnum:	PINSEL_FUNC_0,
				Pinmode:	PINSEL_PINMODE_TRISTATE,
				OpenDrain:	PINSEL_PINMODE_NORMAL
		};

		// Active low
		bool isInputClear()			{ return gpioPinRead(AVC_RX_PIN); }
		bool isInputSet()			{ return !isInputClear(); }
		void setStandby(bool isOn)	{ gpioPinWrite(AVC_STB_PIN, isOn); }
		void setEnabled(bool isOn)	{ gpioPinWrite(AVC_EN_PIN, isOn); }
		// Active low
		void setTx(bool isOn)		{ gpioPinWrite(AVC_TX_PIN, !isOn); }

		// Times in timer ticks
		const uint32_t T_StartBit	= timer.uS(	168	);
		const uint32_t T_Bit		= timer.uS(	39	);
		const uint32_t T_Bit_1		= 		T_Bit 	/ 2;
		const uint32_t T_Bit_0		= (4 *	T_Bit) 	/ 5;
		const uint32_t T_BitMeasure	= (T_Bit_1 + T_Bit_0) / 2;

	public:
		typedef enum
		{   // No this is not a mistake, broadcast = 0!
		    AVC_MSG_DIRECT    = 1,
		    AVC_MSG_BROADCAST = 0
		} AvcTransmissionMode;

		void begin();

		bool readBit();

		uint32_t readBits(uint8_t n);

		void readMessage();

		void onTimerCallback();

		static AVCLanDrv* instance;
};
