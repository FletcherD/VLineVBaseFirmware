extern "C" {
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "LPC17xx.h"
}

uint32_t gpioPinRead(uint8_t portNum, uint8_t pinNum);
uint32_t gpioPinRead(PINSEL_CFG_Type pinCfg);

void gpioPinSet(PINSEL_CFG_Type pinCfg);
void gpioPinClear(PINSEL_CFG_Type pinCfg);
void gpioPinWrite(PINSEL_CFG_Type pinCfg, bool value);

void gpioPinSet(uint8_t portNum, uint8_t pinNum);
void gpioPinClear(uint8_t portNum, uint8_t pinNum);
void gpioPinWrite(uint8_t portNum, uint8_t pinNum, bool value);
