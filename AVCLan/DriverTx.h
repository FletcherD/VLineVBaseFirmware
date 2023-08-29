#ifndef DRIVERTX_H_
#define DRIVERTX_H_

#include "cmsis_device.h"
extern "C" {
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
}
#include "timer.h"
#include "uart.h"
#include "util.h"

#include "DriverBase.h"
#include <MessageRaw.h>

#include <vector>

class DriverTx : public virtual DriverBase {
	private:
		typedef uint32_t Time;

		typedef void (DriverTx::*State)();
		State state;

		void state_Idle();
		void state_StartBit();
		void state_PeriodOn();
		void state_PeriodOff();
		void state_GetAck();
		void state_EndPause();

		// ------------------------

		std::queue<MessageRaw> sendQueue;
		uint32_t sendLengthBits;
		uint32_t sendBitPos;
		Time startTime;

		bool getNextBit();

		void messageDone();

		virtual void endTransmit() {};

	public:
		AckValue ackResult;

		DriverTx(p_timer);
		virtual ~DriverTx() {};

		void queueMessage(MessageRaw);

		bool isMessageWaiting();

		void startTransmit();

		void onTimerCallback();
};

#endif
