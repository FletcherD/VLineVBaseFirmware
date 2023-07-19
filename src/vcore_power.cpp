#include "vcore_power.h"
#include "util.h"

extern "C" {
#include "GPIO_LPC17xx.h"
#include "lpc17xx_clkpwr.h"
}
// ----------------------------------------------------------------------------

power::power()
{
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCGPIO, ENABLE);

  GPIO_SetDir(POWER_PORT_NUMBER, POWER_USB5V_PIN, GPIO_DIR_OUTPUT);
  GPIO_SetDir(POWER_PORT_NUMBER, POWER_VCORE_3P3V_PIN, GPIO_DIR_OUTPUT);
  GPIO_SetDir(POWER_PORT_NUMBER, POWER_VCORE_5V_PIN, GPIO_DIR_OUTPUT);

  // Start with power turned off
  turn_off(PowerNode::Usb5v);
  turn_off(PowerNode::Vcore_3p3v);
  turn_off(PowerNode::Vcore_5v);
}

void
power::turn_on(PowerNode powerNode)
{
	GPIO_PinWrite(POWER_PORT_NUMBER, PowerPinList[powerNode], 0);
}

void
power::turn_off(PowerNode powerNode)
{
	GPIO_PinWrite(POWER_PORT_NUMBER, PowerPinList[powerNode], 1);
}
// ----------------------------------------------------------------------------
