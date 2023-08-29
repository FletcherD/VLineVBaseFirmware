#include "util.h"

template<> uint32_t swapBytes(uint32_t num) {
	return __REV(num);
}
template<> uint16_t swapBytes(uint16_t num) {
	return __REV16(num);
}

void pinConfigure(PinCfgType pinCfg) {
	PIN_Configure(
			pinCfg.Portnum,
			pinCfg.Pinnum,
			pinCfg.Funcnum,
			pinCfg.Pinmode,
			pinCfg.OpenDrain
			);
	GPIO_SetDir(pinCfg.Portnum, pinCfg.Pinnum, pinCfg.GpioDir);
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
