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
#include "IEBusMessage.h"

#include <vector>
#include <memory>

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
	void state_CheckCollision();

	// ------------------------

	uint32_t sendLengthBits;
	Time startTime;

	std::queue<std::shared_ptr<IEBusMessage>> sendQueue;

	bool getBit();
	void checkMessageDone();
	void messageDone();

public:
	DriverTx(p_timer);
	virtual ~DriverTx() {};

	void queueMessage(const std::shared_ptr<IEBusMessage>&);

	bool isMessageWaiting();

	void prepareTransmit();

	void startTransmit();

	void onTimerCallback();
};

#endif
