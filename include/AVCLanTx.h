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
#include "AVCLanMsg.h"

class AVCLanTx : public virtual AVCLanDrvBase {
	private:
		typedef uint32_t Time;
		enum SendEvent {
			TIMER_TRIGGERED,
			SEND_REQUESTED
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

		typedef void (AVCLanTx::*State)(SendEvent);
		State state;

		void state_Idle(SendEvent i);
		void state_StartBit(SendEvent i);
		void state_PeriodOff(SendEvent i);
		void state_PeriodOn(SendEvent i);
		void state_EndWait(SendEvent i);

		// ------------------------

		std::queue<AVCLanMsg> sendQueue;
		uint32_t bufPos = 0;

		bool getNextBit();

		// Active low
		void setTx(bool isOn)		{
			gpioPinWrite(AVC_TX_PIN, !isOn);
		}

		virtual void txEnd() = 0;

	public:
		AVCLanTx(p_timer);
		virtual ~AVCLanTx() {};

		void queueMessage(AVCLanMsg);

		bool isMessageWaiting();

		void startSend();

		void onTimerCallback();
};

#endif
