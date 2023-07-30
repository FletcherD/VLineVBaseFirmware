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

		typedef void (AVCLanTx::*State)(SendEvent);
		State state;

		void state_Idle(SendEvent i);
		void state_StartBit(SendEvent i);
		void state_PeriodOff(SendEvent i);
		void state_PeriodOn(SendEvent i);
		void state_EndWait(SendEvent i);

		// ------------------------

		std::queue<AVCLanMsg> sendQueue;
		uint32_t sendLengthBits;
		uint32_t sendBitPos;

		uint32_t sendX[256];
		uint8_t sendY[256];
		uint32_t sendI = 0;

		bool getNextBit();

		void setTxPinState(bool isOn);

		virtual void endTransmit() {};

	public:
		AVCLanTx(p_timer);
		virtual ~AVCLanTx() {};

		void queueMessage(AVCLanMsg);

		bool isMessageWaiting();

		void startTransmit();

		void onTimerCallback();
};

#endif
