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

#include <vector>

class AVCLanTx : public virtual AVCLanDrvBase {
	private:
		typedef uint32_t Time;

		typedef void (AVCLanTx::*State)();
		State state;

		void state_Idle();
		void state_StartBit();
		void state_PeriodOn();
		void state_PeriodOff();
		void state_GetAck();
		void state_EndPause();

		// ------------------------

		std::queue<AVCLanMsg> sendQueue;
		uint32_t sendLengthBits;
		uint32_t sendBitPos;
		Time startTime;

		bool getNextBit();

		void messageDone();

		void setTxPinState(bool isOn);

		virtual void endTransmit() {};

	public:
		AVCLanMsg::AckValue ackResult;

		AVCLanTx(p_timer);
		virtual ~AVCLanTx() {};

		void queueMessage(AVCLanMsg);

		bool isMessageWaiting();

		void startTransmit();

		void onTimerCallback();
};

#endif
