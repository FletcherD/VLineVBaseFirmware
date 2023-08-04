#include "AVCLanDrvRxTx.h"

AVCLanDrvRxTx* AVCLanDrvRxTx::instance;

AVCLanDrvRxTx::AVCLanDrvRxTx(p_timer timer)
: AVCLanDrvBase(timer),
  AVCLanTx(timer),
  AVCLanRx(timer)
{
	AVCLanDrvRxTx::instance = this;
}

AVCLanDrvRxTx::~AVCLanDrvRxTx() {
}

void AVCLanDrvRxTx::onTimerCallback() {
	if(operatingMode == IDLE) {
		startReceive();
		AVCLanRx::onTimerCallback();
	} else if(operatingMode == RECEIVE) {
		AVCLanRx::onTimerCallback();
	} else if(operatingMode == TRANSMIT) {
		AVCLanTx::onTimerCallback();
	}

	timer.clearInterrupt();
}

void AVCLanDrvRxTx::sendMessage(AVCLanMsg message) {
	AVCLanTx::queueMessage(message);
	if( operatingMode == IDLE ) {
		startTransmit();
	}
}

void AVCLanDrvRxTx::endReceive() {
	startIdle();
}
void AVCLanDrvRxTx::endTransmit() {
	startIdle();
}

void AVCLanDrvRxTx::startTransmit() {
	operatingMode = TRANSMIT;
	timer.setCaptureInterruptEnabled(false);
	timer.setTimerInterruptEnabled(true);
	AVCLanTx::startTransmit();
}
void AVCLanDrvRxTx::startReceive() {
	operatingMode = RECEIVE;
	timer.setCaptureInterruptEnabled(true);
	timer.setTimerInterruptEnabled(true);
}

void AVCLanDrvRxTx::startIdle() {
	if(AVCLanTx::isMessageWaiting()) {
		startTransmit();
	} else {
		operatingMode = IDLE;
		timer.setCaptureInterruptEnabled(true);
		timer.setTimerInterruptEnabled(false);
	}
}

void AVCLanDrvRxTx::messageReceived(AVCLanMsg message) {
	const char* messageStr = message.toString();
	uartOut.printf(messageStr);

	messageReceivedCallback(message);
}

extern "C" {
void TIMER2_IRQHandler(void) {
	AVCLanDrvRxTx::instance->onTimerCallback();
}
}