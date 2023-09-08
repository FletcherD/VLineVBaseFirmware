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
		/* The driver is implemented using a simple state machine.
		 * Each state is a function, and the current state is stored in a function pointer.
		 * When the timer tells us a certain amount of time has passed,
		 * we set the output pin appropriately and move to the next state.
		 */

		typedef void (DriverTx::*State)();
		State state;

		void state_Idle();
		void state_StartBit();
		void state_PeriodOn();
		void state_PeriodOff();
		void state_GetAck();
		void state_EndPause();

		// ------------------------

		std::queue<MessageRawPtr> sendQueue;
		MessageRawPtr curMessage;
		uint32_t sendLengthBits;
		uint32_t sendBitPos;
		Time startTime;

		bool getNextBit();

	//TODO remove
	public:

		void messageDone();

		virtual void endTransmit() {};

	public:
		AckValue ackResult;

		DriverTx(p_timer);
		virtual ~DriverTx() {};

		void queueMessage(MessageRawPtr);

		bool isMessageWaiting();

		void prepareTransmit();
		void startTransmit();

		void onTimerCallback();
};

#endif
