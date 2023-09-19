extern "C" {
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
#include <stdio.h>
#include <stdarg.h>
}

#ifndef UTIL_H_
#define UTIL_H_

template<typename T> T swapBytes(T num);

template<typename T> T swapBytes(T num) {
	return num;
}

struct PinCfgType {
	uint8_t PortNum;	/**< Port Number, should be PINSEL_PORT_x,
						where x should be in range from 0 to 4 */
	uint8_t PinNum;		/**< Pin Number, should be PINSEL_PIN_x,
						where x should be in range from 0 to 31 */
	uint8_t FuncNum = PIN_FUNC_0;	/**< Function Number, should be PINSEL_FUNC_x,
						where x should be in range from 0 to 3 */
	uint8_t PinMode = PIN_PINMODE_TRISTATE;	/**< Pin Mode, should be:
						- PIN_PINMODE_PULLUP: Internal pull-up resistor
						- PIN_PINMODE_TRISTATE: Tri-state
						- PIN_PINMODE_PULLDOWN: Internal pull-down resistor */
	uint8_t OpenDrain = PIN_PINMODE_NORMAL;	/**< OpenDrain mode, should be:
						- PIN_PINMODE_NORMAL: Pin is in the normal (not open drain) mode
						- PIN_PINMODE_OPENDRAIN: Pin is in the open drain mode */
	uint8_t GpioDir = GPIO_DIR_OUTPUT;	/** GPIO_DIR_INPUT, GPIO_DIR_OUTPUT */
	bool ActiveLow = false;
};

void pinConfigure(PinCfgType);

uint32_t pinMask(uint8_t pinNum);

uint32_t gpioPinRead(PinCfgType);

void gpioPinSet(PinCfgType);
void gpioPinClear(PinCfgType);
void gpioPinWrite(PinCfgType, bool value);

void gpioPinSet(uint8_t portNum, uint8_t pinNum);
void gpioPinClear(uint8_t portNum, uint8_t pinNum);
void gpioPinWrite(uint8_t portNum, uint8_t pinNum, bool value);

#endif
