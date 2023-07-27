#ifndef AVCLANTX_H_
#define AVCLANTX_H_

#include "cmsis_device.h"
extern "C" {
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
}
#include "timer.h"
#include "uart.h"
#include "util.h"

#include "AVCLanDrv.h"

class AVCLanTx : public virtual AVCLanDrvBase {
	private:
		typedef uint32_t Time;
		enum SendEventType {
			TIMER_MATCH
		};
		struct SendEvent {
			Time time;
			SendEventType type;
		};
		Time lastEventTime;


		PinCfgType AVC_TX_PIN = {
				Portnum: 	0,
				Pinnum:		5,
				Funcnum:	PIN_FUNC_0,
				Pinmode:	PIN_PINMODE_TRISTATE,
				OpenDrain:	PIN_PINMODE_OPENDRAIN,
				GpioDir:	GPIO_DIR_INPUT
		};

		typedef void (AVCLanTx::*State)(AVCLanTx);
		State state;

		void state_Idle(SendEvent i);
		void state_StartBit(SendEvent i);
		void state_OffPeriod(SendEvent i);
		void state_OnPeriod(SendEvent i);

		// ------------------------

		struct SendData {
			uint8_t data;
			uint32_t length;
		};
		std::queue<SendData> sendBuf;

		// Active low
		void setTx(bool isOn)		{
			gpioPinWrite(AVC_TX_PIN, !isOn);
		}

	public:
		AVCLanTx(p_timer);

		void onTimerCallback();
};

#endif
