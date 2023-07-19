#include "cmsis_device.h"
extern "C" {
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
}
#include "timer.h"
#include "uart.h"
#include "util.h"

class AVCLanDrv{
	private:
		typedef uint32_t Time;
		enum InputType {
			RISING_EDGE,
			FALLING_EDGE,
			TIMEOUT
		};

		struct InputEvent {
			Time time;
			InputType type;
		};

		typedef void (AVCLanDrv::*State)(InputEvent);
		State state;

		void state_Idle(InputEvent i);
		void state_StartBit(InputEvent i);
		void state_WaitForBit(InputEvent i);
		void state_MeasureBit(InputEvent i);

		uint8_t messageBuf[256];
		uint32_t bufPos;
		void receiveBit(bool bit);

		p_timer timer = p_timer(2);

		PinCfgType AVC_RX_PIN = {
				Portnum: 	0,
				Pinnum:		4,
				// Timer Capture function
				Funcnum:	PIN_FUNC_3,
				Pinmode:	PIN_PINMODE_PULLUP,
				OpenDrain:	PIN_PINMODE_NORMAL
		};
		PinCfgType AVC_TX_PIN = {
				Portnum: 	0,
				Pinnum:		5,
				Funcnum:	PIN_FUNC_0,
				Pinmode:	PIN_PINMODE_TRISTATE,
				OpenDrain:	PIN_PINMODE_OPENDRAIN
		};
		PinCfgType AVC_STB_PIN = {
				Portnum: 	2,
				Pinnum:		5,
				Funcnum:	PIN_FUNC_0,
				Pinmode:	PIN_PINMODE_TRISTATE,
				OpenDrain:	PIN_PINMODE_OPENDRAIN
		};
		PinCfgType AVC_EN_PIN = {
				Portnum: 	2,
				Pinnum:		6,
				Funcnum:	PIN_FUNC_0,
				Pinmode:	PIN_PINMODE_TRISTATE,
				OpenDrain:	PIN_PINMODE_NORMAL
		};
		PinCfgType AVC_ERR_PIN = {
				Portnum: 	2,
				Pinnum:		7,
				Funcnum:	PIN_FUNC_0,
				Pinmode:	PIN_PINMODE_TRISTATE,
				OpenDrain:	PIN_PINMODE_NORMAL
		};

		uint32_t lastEventTime;

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
		const uint32_t T_Timeout	= 10000;

	public:
		typedef enum
		{   // No this is not a mistake, broadcast = 0!
		    AVC_MSG_DIRECT    = 1,
		    AVC_MSG_BROADCAST = 0
		} AvcTransmissionMode;

		void begin();

		void onTimerCallback();

		static AVCLanDrv* instance;
};
