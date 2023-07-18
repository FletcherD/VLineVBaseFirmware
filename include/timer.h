/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "cmsis_device.h"
extern "C" {
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"
}


// ----------------------------------------------------------------------------

class p_timer
{
	// Timer 0
private:
	LPC_TIM_TypeDef* lpcTimer;
public:
	typedef uint32_t ticks_t;
	const ticks_t tickRate = SystemCoreClock;
	ticks_t uS(uint32_t uS) {
		return uS * (tickRate / 1000000);
	}

	p_timer()
	{
		lpcTimer = LPC_TIM0;
		TIM_TIMERCFG_Type lpcTimerConfig;
		TIM_ConfigStructInit(TIM_TIMER_MODE, &lpcTimerConfig);
		lpcTimerConfig.PrescaleOption = TIM_PRESCALE_USVAL;
		lpcTimerConfig.PrescaleValue = 1;
		TIM_Init(lpcTimer, TIM_TIMER_MODE, &lpcTimerConfig);
		LPC_TIM0->PR = 100;

		TIM_Cmd(lpcTimer, FunctionalState::ENABLE);
	}

	void reset() {
		lpcTimer->PC = 0;
	}
	ticks_t getPrescaleTickRate() {
		return CLKPWR_GetPCLK(CLKPWR_PCLKSEL_TIMER0);
	}
	uint64_t getBigTicks()	{
		return lpcTimer->TC;
	}
	ticks_t	getTicks()	{
		return lpcTimer->PC;
	}
	void sleep(ticks_t duration) {
		reset();
		while(getTicks() < duration);
	}

	void
	setupRepeatingInterrupt(ticks_t interval)
	{
		TIM_MATCHCFG_Type timerMatchConfig;
		timerMatchConfig.MatchChannel = 0;
		timerMatchConfig.MatchValue = interval;
		timerMatchConfig.IntOnMatch = ENABLE;
		timerMatchConfig.ResetOnMatch = ENABLE;
		timerMatchConfig.StopOnMatch = DISABLE;
		timerMatchConfig.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
		TIM_ConfigMatch(lpcTimer, &timerMatchConfig);

		NVIC_EnableIRQ(TIMER0_IRQn);
	}
};


class timer
{
public:
  typedef uint32_t ticks_t;
  static constexpr ticks_t FREQUENCY_HZ = 1000u;

private:
  static volatile ticks_t ms_delayCount;

public:
  // Default constructor
  timer() = default;

  void
  start(void);

  static void
  sleep(ticks_t ticks);

  static void
  tick(void);
};

// ----------------------------------------------------------------------------

#endif // TIMER_H_
