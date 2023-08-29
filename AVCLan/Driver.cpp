#include <Driver.h>
#include <VCoreCommunication.h>
#include <functional>
#include "diag/trace.h"

Driver* Driver::instance;

Driver::Driver(p_timer timer)
: DriverBase(timer),
  DriverTx(timer),
  DriverRx(timer)
{
	Driver::instance = this;
}

Driver::~Driver() {
}

void Driver::onTimerCallback() {
	if(operatingMode == IDLE) {
		startReceive();
		DriverRx::onTimerCallback();
	} else if(operatingMode == RECEIVE) {
		DriverRx::onTimerCallback();
	} else if(operatingMode == TRANSMIT) {
		DriverTx::onTimerCallback();
	}

	timer.clearInterrupt();
}

void Driver::sendMessage(MessageRaw message) {
<<<<<<< HEAD
=======
	char messageStr[256];
	message.toString(messageStr);
	trace_printf("Sending message: %s", messageStr);
>>>>>>> 93eff14 (Interim state to allow reading of the code)
	DriverTx::queueMessage(message);
	if( operatingMode == IDLE ) {
		startTransmit();
	}
}

void Driver::endReceive() {
	startIdle();
}
void Driver::endTransmit() {
	startIdle();
}

void Driver::startTransmit() {
	operatingMode = TRANSMIT;
	timer.setCaptureInterruptEnabled(false);
	timer.setTimerInterruptEnabled(true);
	DriverTx::startTransmit();
}
void Driver::startReceive() {
	operatingMode = RECEIVE;
	timer.setCaptureInterruptEnabled(true);
	timer.setTimerInterruptEnabled(true);
}

void Driver::startIdle() {
	if(DriverTx::isMessageWaiting()) {
		startTransmit();
	} else {
		operatingMode = IDLE;
		timer.setCaptureInterruptEnabled(true);
		timer.setTimerInterruptEnabled(false);
	}
}

void Driver::messageReceived(MessageRaw message) {
/*
	char messageStr[128];
	message.toString(messageStr);
	trace_printf("%d %s", timer.getTicks(), messageStr);
*/

	receiveQueue.push(message);
	while(receiveQueue.size() > 8) {
		receiveQueue.pop();
	}
}

extern "C" {
void TIMER2_IRQHandler(void) {
	Driver::instance->onTimerCallback();
}
}
