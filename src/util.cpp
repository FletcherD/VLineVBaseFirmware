#include "util.h"

void pinConfigure(PinCfgType pinCfg) {
	PIN_Configure(
			pinCfg.Portnum,
			pinCfg.Pinnum,
			pinCfg.Funcnum,
			pinCfg.Pinmode,
			pinCfg.OpenDrain
			);
}

uint32_t pinMask(uint8_t pinNum) {
	return (1UL)<<pinNum;
}

uint32_t gpioPinRead(PinCfgType pinCfg) {
	return GPIO_PinRead(pinCfg.Portnum, pinCfg.Pinnum);
}
void gpioPinWrite(PinCfgType pinCfg, bool value) {
	GPIO_PinWrite(pinCfg.Portnum, pinCfg.Pinnum, value);
}
