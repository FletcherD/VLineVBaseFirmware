#include "DriverBase.h"

DriverBase::DriverBase(p_timer timer)
	: timer(timer)
{
}

bool DriverBase::getRxPinState() {
	return gpioPinRead(AVC_RX_PIN);
}
void DriverBase::setTxPinState(bool isOn) {
	// Active low
	gpioPinWrite(AVC_TX_PIN, !isOn);
}

void DriverBase::setStandby(bool isOn)	{
	gpioPinWrite(AVC_STB_PIN, isOn);
}
void DriverBase::setEnabled(bool isOn)	{
	// Active low
	gpioPinWrite(AVC_EN_PIN, !isOn);
}
