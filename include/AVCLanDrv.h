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

struct SendData {
	uint16_t time;
	uint16_t data;
};

static constexpr uint32_t sendBufSize = 128;
static SendData sendBuf[1024];
static uint32_t sendPos = 0;

class AVCLanDrv{
	public:

		p_timer timer = p_timer(2);
	private:
		typedef uint32_t Time;
		enum InputType {
			RISING_EDGE,
			FALLING_EDGE
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


		static constexpr uint8_t messageBufLen = 128;
		uint8_t messageBuf[messageBufLen];
		uint32_t bufPos;

		AVCLanMsg thisMsg;

		void receiveBit(bool bit);

		void messageEnd();

		void resetBuffer();

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
		uint32_t lastEventTime;


		void setStandby(bool isOn)	{ gpioPinWrite(AVC_STB_PIN, isOn); }
		// Active low
		void setEnabled(bool isOn)	{ gpioPinWrite(AVC_EN_PIN, !isOn); }
		// Active low
		void setTx(bool isOn)		{ gpioPinWrite(AVC_TX_PIN, !isOn); }

		// Times in timer ticks
		const uint32_t T_StartBit	= 170;	//timer.uS(	168	);
		const uint32_t T_Bit		= 39;	//timer.uS(	39	);
		const uint32_t T_Bit_1		= 		T_Bit 	/ 2;
		const uint32_t T_Bit_0		= (4 *	T_Bit) 	/ 5;
		const uint32_t T_BitMeasure	= 27;	//(T_Bit_1 + T_Bit_0) / 2;
		const uint32_t T_Timeout	= 200;	//timer.uS( 300 );

	public:
		AVCLanDrv();

		void begin();

		void onTimerCallback();

		static AVCLanDrv* instance;
};
