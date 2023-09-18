#include <DriverBase.h>

DriverBase::DriverBase(p_timer& timer)
	: timer(timer)
{
	canTxTime = timer.getTicks();

	timer.setupTimerInterrupt(1000000);
	timer.setupCaptureInterrupt();

	startIdle();
}

bool DriverBase::getRxState() {
	return gpioPinRead(AvcRxPin);
}
void DriverBase::setTxState(bool isOn) {
	// Active low
	gpioPinWrite(AvcTxPin, !isOn);
}

void DriverBase::setStandby(bool isOn)	{
	gpioPinWrite(AvcStandbyPin, isOn);
}
void DriverBase::setEnabled(bool isOn)	{
	// Active low
	gpioPinWrite(AvcEnablePin, !isOn);
}

void DriverBase::sendMessage() {
	// Do the time-consuming stuff first;
	// There's a chance a message will start to come in while we prepare,
	// and then we have to wait until it's done before we start
	prepareTransmit();
	while(operatingMode != IDLE || timer.getTicks() < canTxTime) {}

	uartOut.setEnabled(false);
	timer.setCaptureInterruptEnabled(false);
	timer.setTimerInterruptEnabled(true);
	operatingMode = TRANSMIT;
	startTransmit();
}

void DriverBase::endTransmit() {
	canTxTime = timer.getTicks() + T_TxWait;
	startIdle();
}

void DriverBase::startReceive() {
	uartOut.setEnabled(false);
	timer.setCaptureInterruptEnabled(true);
	timer.setTimerInterruptEnabled(false);
	operatingMode = RECEIVE;
}

void DriverBase::endReceive() {
	canTxTime = timer.getTicks() + T_TxWait;
	startIdle();
}

void DriverBase::startIdle() {
	resetBuffer();
	timer.setCaptureInterruptEnabled(true);
	timer.setTimerInterruptEnabled(false);
	uartOut.setEnabled(true);
	operatingMode = IDLE;
}

void DriverBase::nextBit() {
	curBit++;

	if(curBit == (curField->bitOffset + curField->bitLength)) {
		curField++;
		if((!curField->isAck) && (!curField->isParity)) {
			curParity = false;
		}
	}
}

void DriverBase::resetBuffer() {
	curMessage.reset(new IEBusMessage);
	curField = IEBusFields.cbegin();
	curBit = 0;
	curParity = false;
}