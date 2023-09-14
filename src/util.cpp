#include "util.h"

template<> uint32_t swapBytes(uint32_t num) {
	return __REV(num);
}
template<> uint16_t swapBytes(uint16_t num) {
	return __REV16(num);
}

void pinConfigure(PinCfgType pinCfg) {
	GPIO_SetDir(pinCfg.PortNum, pinCfg.PinNum, pinCfg.GpioDir);
	PIN_Configure(
			pinCfg.PortNum,
			pinCfg.PinNum,
			pinCfg.FuncNum,
			pinCfg.PinMode,
			pinCfg.OpenDrain
			);
}

uint32_t pinMask(uint8_t pinNum) {
	return (1UL)<<pinNum;
}

uint32_t gpioPinRead(PinCfgType pinCfg) {
	return GPIO_PinRead(pinCfg.PortNum, pinCfg.PinNum);
}
void gpioPinWrite(PinCfgType pinCfg, bool value) {
	GPIO_PinWrite(pinCfg.PortNum, pinCfg.PinNum, value);
}
