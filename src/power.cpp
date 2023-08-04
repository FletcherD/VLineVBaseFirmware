#include "power.h"
#include "util.h"

extern "C" {
#include "GPIO_LPC17xx.h"
#include "lpc17xx_clkpwr.h"
}
// ----------------------------------------------------------------------------

constexpr PinCfgType power::VCore5V;
constexpr PinCfgType power::VCore3P3V;
constexpr PinCfgType power::VBase5V;
constexpr PinCfgType power::Usb5V;
constexpr PinCfgType power::AudioAmp;

power::power()
{
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPIO, ENABLE);

	pinConfigure(power::VCore5V);
	pinConfigure(power::VCore3P3V);
	pinConfigure(power::VBase5V);
	pinConfigure(power::Usb5V);
	pinConfigure(power::AudioAmp);

	// Start with power turned off
	turn_off(power::VCore5V);
	turn_off(power::VCore3P3V);
	turn_off(power::VBase5V);
	turn_off(power::Usb5V);
	turn_off(power::AudioAmp);
}

void
power::turn_on(PinCfgType powerPin)
{
	// Power pins are all active low
	GPIO_PinWrite(powerPin.Portnum, powerPin.Pinnum, 0);
}

void
power::turn_off(PinCfgType powerPin)
{
	GPIO_PinWrite(powerPin.Portnum, powerPin.Pinnum, 1);
}
// ----------------------------------------------------------------------------
