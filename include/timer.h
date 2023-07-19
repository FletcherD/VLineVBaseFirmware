#ifndef TIMER_H_
#define TIMER_H_

#include "cmsis_device.h"
extern "C" {
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"
}

// ----------------------------------------------------------------------------

static constexpr LPC_TIM_TypeDef* LpcTimers[] = {
	LPC_TIM0, LPC_TIM1, LPC_TIM2, LPC_TIM3	};
static constexpr IRQn LpcTimerIRQs[] = {
	TIMER0_IRQn, TIMER1_IRQn, TIMER2_IRQn, TIMER3_IRQn	};
static constexpr uint32_t LpcTimerPClks[] = {
	CLKPWR_PCLKSEL_TIMER0, CLKPWR_PCLKSEL_TIMER1, CLKPWR_PCLKSEL_TIMER2, CLKPWR_PCLKSEL_TIMER3	};

class p_timer
{
private:

	const uint8_t timerN;
	LPC_TIM_TypeDef* lpcTimer;

public:

	const uint32_t tickRate = SystemCoreClock;

	uint32_t uS(uint32_t uS) {
		return uS * (tickRate / 1000000);
	}

	p_timer(uint8_t timerN);

	void reset();

	uint32_t getPrescaleTickRate();

	uint32_t getBigTicks();

	uint32_t getTicks();

	void sleep(uint32_t duration);

	void setupRepeatingInterrupt(uint32_t interval);
	void setupCaptureInterrupt();
	void clearInterrupt();
};

#endif // TIMER_H_
