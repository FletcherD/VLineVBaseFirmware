#include "util.h"

uint32_t gpioPinRead(uint8_t portNum, uint8_t pinNum) {
	return GPIO_ReadValue(portNum) & (1UL<<pinNum);
}
uint32_t gpioPinRead(PINSEL_CFG_Type pinCfg) {
	return GPIO_ReadValue(pinCfg.Portnum) & (1UL<<pinCfg.Pinnum);
}

void gpioPinSet(PINSEL_CFG_Type pinCfg) {
	GPIO_SetValue(pinCfg.Portnum, 1UL<<pinCfg.Pinnum);
}
void gpioPinClear(PINSEL_CFG_Type pinCfg) {
	GPIO_ClearValue(pinCfg.Portnum, 1UL<<pinCfg.Pinnum);
}
void gpioPinWrite(PINSEL_CFG_Type pinCfg, bool value) {
	if(value)
		gpioPinSet(pinCfg);
	else
		gpioPinClear(pinCfg);
}

void gpioPinSet(uint8_t portNum, uint8_t pinNum) {
	GPIO_SetValue(portNum, 1UL<<pinNum);
}
void gpioPinClear(uint8_t portNum, uint8_t pinNum) {
	GPIO_ClearValue(portNum, 1UL<<pinNum);
}
void gpioPinWrite(uint8_t portNum, uint8_t pinNum, bool value) {
	if(value)
		gpioPinSet(portNum, pinNum);
	else
		gpioPinClear(portNum, pinNum);
}
