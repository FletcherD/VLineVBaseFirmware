#include "power.h"
#include "util.h"

extern "C" {
#include "GPIO_LPC17xx.h"
#include "lpc17xx_clkpwr.h"
}
// ----------------------------------------------------------------------------

power::power()
{
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPIO, ENABLE);

	pinConfigure(power::VCore5V);
	pinConfigure(power::VCore3P3V);
	pinConfigure(power::VBase5V);
	pinConfigure(power::Usb5V);
	pinConfigure(power::AudioAmp);
	pinConfigure(power::VideoReset);
	pinConfigure(power::VideoSel);

	// Start with power turned off
	turnOff(power::VCore5V);
	turnOff(power::VCore3P3V);
	turnOff(power::VBase5V);
	turnOff(power::Usb5V);
	turnOff(power::AudioAmp);
	turnOff(power::VideoReset);
	turnOff(power::VideoSel);
}

void
power::turnOn(PinCfgType powerPin)
{
	gpioPinWrite(powerPin, true);
}

void
power::turnOff(PinCfgType powerPin)
{
	gpioPinWrite(powerPin, false);
}
// ----------------------------------------------------------------------------
