#include "timer.h"
#include "cortexm/exception-handlers.h"

// ----------------------------------------------------------------------------

p_timer::p_timer(uint8_t timerN) : timerN(timerN), lpcTimer(LpcTimers[timerN])
{
	TIM_TIMERCFG_Type lpcTimerConfig;
	TIM_ConfigStructInit(TIM_TIMER_MODE, &lpcTimerConfig);
	lpcTimerConfig.PrescaleOption = TIM_PRESCALE_TICKVAL;
	lpcTimerConfig.PrescaleValue = 100;
	TIM_Init(lpcTimer, TIM_TIMER_MODE, &lpcTimerConfig);

	setCaptureInterruptEnabled(false);
	setTimerInterruptEnabled(false);
	clearInterrupt();

	NVIC_EnableIRQ(LpcTimerIRQs[timerN]);
	NVIC_SetPriority(LpcTimerIRQs[timerN], 0);
	TIM_Cmd(lpcTimer, FunctionalState::ENABLE);
}

void p_timer::reset() {
	lpcTimer->TC = 0;
}
uint32_t p_timer::getPrescaleTickRate() {
	return CLKPWR_GetPCLK(LpcTimerPClks[timerN]);
}
uint32_t p_timer::ticksPerS() {
	return getPrescaleTickRate() / (lpcTimer->PR+1);
}
uint32_t p_timer::uS(float uS) {
	return (ticksPerS() * uS) / 1000000;
}

uint32_t p_timer::getTicks() {
	return lpcTimer->TC;
}
void p_timer::sleep(uint32_t duration) {
	uint32_t waitUntil = getTicks() + duration;
	while(getTicks() < waitUntil);
}

void
p_timer::setupTimerInterrupt(uint32_t duration)
{
	TIM_MATCHCFG_Type timerMatchConfig;
	timerMatchConfig.MatchChannel = 0;
	timerMatchConfig.MatchValue = (lpcTimer->TC) + duration;
	timerMatchConfig.IntOnMatch = ENABLE;
	timerMatchConfig.ResetOnMatch = DISABLE;
	timerMatchConfig.StopOnMatch = DISABLE;
	timerMatchConfig.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	TIM_ConfigMatch(lpcTimer, &timerMatchConfig);
}
void
p_timer::updateTimer(uint32_t duration)
{
	TIM_UpdateMatchValue(lpcTimer, 0, (lpcTimer->TC) + duration);
}
void
p_timer::updateTimerAbsolute(uint32_t time)
{
	TIM_UpdateMatchValue(lpcTimer, 0, time);
}
void
p_timer::setTimerInterruptEnabled(bool isEnabled)
{
	uint32_t bitMask = TIM_INT_ON_MATCH(0);
	if(isEnabled) {
		lpcTimer->MCR |= bitMask;
	} else {
		lpcTimer->MCR &= (~bitMask);
	}
}

void
p_timer::setupCaptureInterrupt()
{
	TIM_CAPTURECFG_Type timerCapCfg;
	timerCapCfg.CaptureChannel = 0;
	timerCapCfg.FallingEdge = 1;
	timerCapCfg.RisingEdge = 1;
	timerCapCfg.IntOnCaption = 1;
	TIM_ConfigCapture(lpcTimer, &timerCapCfg);
}

void
p_timer::setCaptureInterruptEnabled(bool isEnabled)
{
	uint32_t bitMask = TIM_INT_ON_CAP(0);
	if(isEnabled) {
		lpcTimer->CCR |= bitMask;
	} else {
		lpcTimer->CCR &= (~bitMask);
	}
}


void p_timer::setIrqEnabled(bool isEnabled) {
	if (isEnabled) {
		NVIC_EnableIRQ(LpcTimerIRQs[timerN]);
	} else {
		NVIC_DisableIRQ(LpcTimerIRQs[timerN]);
	}
}
void
p_timer::clearInterrupt()
{
	lpcTimer->IR = 0xffffffff;
}
