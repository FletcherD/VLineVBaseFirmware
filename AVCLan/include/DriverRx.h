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
#include "IEBusMessage.h"

#include <memory>

class DriverRx : public virtual DriverBase {
private:
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
	void state_MeasureBit_Rising(InputEvent i);
	void state_WaitToAck(InputEvent i);
	void state_Ack(InputEvent i);

	// ------------------------

	void receiveBit(bool bit);
	void checkMessageDone();
	void messageDone();
	virtual void messageReceived(const std::shared_ptr<IEBusMessage>&) {};

	void onBitError();
	void collisionRecover();

public:

	DriverRx(p_timer&);
	virtual ~DriverRx() = default;

	virtual void onTimerCallback();

	uint32_t totalMsgsReceived = 0;
	uint32_t bitErrorCount = 0;
	uint32_t longestMsg = 0;
};

#endif
