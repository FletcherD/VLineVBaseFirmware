#include <DriverBase.h>

DriverBase::DriverBase(p_timer timer)
	: timer(timer)
{
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
	timer.setCaptureInterruptEnabled(true);
	timer.setTimerInterruptEnabled(false);
	operatingMode = RECEIVE;
}

void DriverBase::endReceive() {
	canTxTime = timer.getTicks() + T_TxWait;
	startIdle();
}

void DriverBase::startIdle() {
	timer.setCaptureInterruptEnabled(true);
	timer.setTimerInterruptEnabled(false);
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