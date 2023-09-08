#include <Driver.h>
#include <VCoreCommunication.h>
#include <functional>
#include "diag/trace.h"

Driver* Driver::instance;

Driver::Driver(p_timer timer)
: DriverBase(timer),
  DriverRx(timer),
  DriverTx(timer)
{
	Driver::instance = this;

	canTxTime = timer.getTicks();
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

void Driver::sendMessage(MessageRawPtr message) {
	DriverTx::queueMessage(message);
}

void Driver::startTransmit() {
	// Do the time consuming stuff first;
	// There's a chance a message will start to come in while we prepare,
	// and then we have to wait until it's done before we start
	DriverTx::prepareTransmit();
	while(operatingMode != IDLE || timer.getTicks() < canTxTime) {}

#ifdef NO_TX
	DriverTx::messageDone();
#else
	timer.setCaptureInterruptEnabled(false);
	timer.setTimerInterruptEnabled(true);
	operatingMode = TRANSMIT;
	DriverTx::startTransmit();
#endif
}

void Driver::endTransmit() {
	canTxTime = timer.getTicks() + T_TxWait;
	startIdle();
}

void Driver::startReceive() {
	operatingMode = RECEIVE;
}

void Driver::endReceive() {
	canTxTime = timer.getTicks() + T_TxWait;
	startIdle();
}

void Driver::startIdle() {
	timer.setCaptureInterruptEnabled(true);
	timer.setTimerInterruptEnabled(false);
	operatingMode = IDLE;
}

void Driver::poll() {
	if(DriverTx::isMessageWaiting() && operatingMode == IDLE) {
		Driver::startTransmit();
	}
}

void Driver::messageReceived(MessageRawPtr message) {
	receiveQueue.push(message);
}

extern "C" {
void TIMER2_IRQHandler(void) {
	Driver::instance->onTimerCallback();
}
}
