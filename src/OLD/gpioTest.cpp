#include <stdio.h>
#include <stdarg.h>
#include "diag/trace.h"
#include "gpioTest.h"
#include "uart.h"

#define PIN_Func(port,regidx) (*((volatile uint32_t *) (&(LPC_PINCON->PINSEL0) + 2U*port + regidx)))
bool pinIsGpio(uint8_t port, uint8_t pin) {
	  uint32_t regidx = 0;
	  uint8_t pinnum = pin;
	  if (pin >= 16) {
	    pinnum   = pin - 16U;
	    regidx = 1U;
	  }
	  uint32_t pinFunc = PIN_Func(port,regidx) & (0x03UL << (pinnum * 2U));
	  return (0 == pinFunc);
}

void gpioTest()
{
	uart uart;
	p_timer timer;

	LPC_GPIO(0)->DIR = 0x00448320;
	LPC_GPIO(1)->DIR = 0x7ffbc313;
	LPC_GPIO(2)->DIR = 0x000039e5;
	LPC_GPIO(3)->DIR = 0x06000000;
	LPC_GPIO(4)->DIR = 0x10000000;
	LPC_GPIO(0)->PIN = 0x3c7f8c3f;
	LPC_GPIO(1)->PIN = 0x98174111;
	LPC_GPIO(2)->PIN = 0x00002765;
	LPC_GPIO(3)->PIN = 0x04000000;
	LPC_GPIO(4)->PIN = 0x10000000;
	LPC_PINCON->PINMODE0 = 0x80aaaaaa;
	LPC_PINCON->PINMODE1 = 0x2aa2aaa8;
	LPC_PINCON->PINMODE2 = 0xa00a020a;
	LPC_PINCON->PINMODE3 = 0xaaaaaa8a;
	LPC_PINCON->PINMODE4 = 0x0a8aaaaa;
	LPC_PINCON->PINMODE7 = 0x00280000;
	LPC_PINCON->PINMODE9 = 0x0a000000;
	LPC_PINCON->PINMODE_OD0 = 0x18180c20;
	LPC_PINCON->PINMODE_OD1 = 0x00100100;
	LPC_PINCON->PINMODE_OD2 = 0x00002020;
	LPC_PINCON->PINMODE_OD3 = 0x04000000;
	LPC_PINCON->PINMODE_OD4 = 0x10000000;

	uint32_t pinMask[NumGpios];
	for(uint32_t i = 0; i < NumGpios; i++) {
		pinMask[i] = 0xffffffff & (~LPC_GPIO(i)->DIR);
		for(uint32_t p = 0; p < 32; p++) {
			if(!pinIsGpio(i, p))
				pinMask[i] &= ~(1UL << p);
		}
		trace_printf("PinMask %d: 0x%08x\n", i, pinMask[i]);
	}

	GPIO_PinWrite(2,5,0);
	GPIO_PinWrite(1,8,0);

	GpioResult gpioBuf[GpioBufLen];
	uint32_t bufUsed = 0;

	uint32_t gpioPrev[NumGpios];
	uint32_t gpioTmp;
	while(1) {
		for(uint32_t i = 0; i < NumGpios; i++) {
			gpioTmp = GPIO_PortRead(i) & pinMask[i];
			if (gpioTmp != gpioPrev[i]) {
				uint32_t gpioDiff = gpioPrev[i] ^ gpioTmp;
				gpioPrev[i] = gpioTmp;

				gpioBuf[bufUsed].gpioNum = i;
				gpioBuf[bufUsed].time = timer.getTicks();
				gpioBuf[bufUsed].data = gpioDiff;
				bufUsed++;

				if(bufUsed == GpioBufLen) {
					for(uint32_t iBuf = 0; iBuf != GpioBufLen; iBuf++) {
						uart.printf("%d 0x%08x %d\r\n", gpioBuf[iBuf].gpioNum, gpioBuf[iBuf].data, gpioBuf[iBuf].time);
					}
					bufUsed = 0;
				}
			}
		}
		//timer.sleep(1);
	}
}
