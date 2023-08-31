#ifndef DRIVERRX_H_
#define DRIVERRX_H_

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

class DriverRx : public virtual DriverBase {
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
		Time lastEventTime;

		/* The driver is implemented using a simple state machine.
		 * Each state is a function, and the current state is stored in a function pointer.
		 * When a transition on the line is detected, the current state is called
		 * with an Event object containing data about the transition.
		 * Based on the time since the last transition, and the current line level,
		 * We decide whether to receive a bit, timeout the message, etc.
		 */

		// AVC-LAN RX state machine
		typedef void (DriverRx::*State)(InputEvent);
		State state;

		void state_Idle(InputEvent i);
		void state_StartBit(InputEvent i);
		void state_WaitForBit(InputEvent i);
		void state_MeasureBit(InputEvent i);

		// ------------------------

		MessageRaw thisMsg;
		uint32_t receiveBitPos;

		void receiveBit(bool bit);
		void resetBuffer();
		void messageEnd();

		void onBitError();

		virtual void endReceive() {};
		virtual void messageReceived(MessageRaw) {};

		// Because we have a very strict time budget,
		// calculate the length on the fly
		// so AVCLanRx knows when it is done
		uint8_t msgDataLength = 0;

	public:
		uint32_t bitErrorCount = 0;
		uint32_t totalMsgCount = 0;

		size_t longestMsg = 0;

		DriverRx(p_timer);
		virtual ~DriverRx() {};

		void onTimerCallback();

};

#endif
