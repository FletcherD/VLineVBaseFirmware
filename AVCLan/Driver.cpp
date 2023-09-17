#include <Driver.h>

Driver* Driver::instance;

Driver::Driver(p_timer& timer)
: DriverBase(timer),
  DriverRx(timer),
  DriverTx(timer)
{
	Driver::instance = this;
}

Driver::~Driver() = default;

void Driver::onTimerCallback() {
	if(operatingMode == IDLE) {
		startReceive();
		DriverRx::onTimerCallback();
	} else if(operatingMode == RECEIVE) {
		DriverRx::onTimerCallback();
	} else if(operatingMode == TRANSMIT) {
		DriverTx::onTimerCallback();
	}
}

void Driver::poll() {
	if(DriverTx::isMessageWaiting() && operatingMode == IDLE) {
		sendMessage();
	}
}

void Driver::messageReceived(const std::shared_ptr<IEBusMessage>& message) {
	receiveQueue.push(message);
}

extern "C" {
void TIMER2_IRQHandler(void) {
	if(Driver::instance != NULL) {
		Driver::instance->onTimerCallback();
	}
	LPC_TIM2->IR = 0xffffffff;
}
}
