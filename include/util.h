extern "C" {
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
}

typedef struct {
	uint8_t Portnum;	/**< Port Number, should be PINSEL_PORT_x,
						where x should be in range from 0 to 4 */
	uint8_t Pinnum;		/**< Pin Number, should be PINSEL_PIN_x,
						where x should be in range from 0 to 31 */
	uint8_t Funcnum;	/**< Function Number, should be PINSEL_FUNC_x,
						where x should be in range from 0 to 3 */
	uint8_t Pinmode;	/**< Pin Mode, should be:
						- PINSEL_PINMODE_PULLUP: Internal pull-up resistor
						- PINSEL_PINMODE_TRISTATE: Tri-state
						- PINSEL_PINMODE_PULLDOWN: Internal pull-down resistor */
	uint8_t OpenDrain;	/**< OpenDrain mode, should be:
						- PINSEL_PINMODE_NORMAL: Pin is in the normal (not open drain) mode
						- PINSEL_PINMODE_OPENDRAIN: Pin is in the open drain mode */
} PinCfgType;

void pinConfigure(PinCfgType);

uint32_t pinMask(uint8_t pinNum);

uint32_t gpioPinRead(PinCfgType pinCfg);

void gpioPinSet(PinCfgType pinCfg);
void gpioPinClear(PinCfgType pinCfg);
void gpioPinWrite(PinCfgType pinCfg, bool value);

void gpioPinSet(uint8_t portNum, uint8_t pinNum);
void gpioPinClear(uint8_t portNum, uint8_t pinNum);
void gpioPinWrite(uint8_t portNum, uint8_t pinNum, bool value);
