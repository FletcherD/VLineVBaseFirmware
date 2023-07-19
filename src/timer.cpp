#include "timer.h"
#include "cortexm/exception-handlers.h"

// ----------------------------------------------------------------------------

p_timer::p_timer(uint8_t timerN) : timerN(timerN), lpcTimer(LpcTimers[timerN])
{
	TIM_TIMERCFG_Type lpcTimerConfig;
	TIM_ConfigStructInit(TIM_TIMER_MODE, &lpcTimerConfig);
	lpcTimerConfig.PrescaleOption = TIM_PRESCALE_USVAL;
	lpcTimerConfig.PrescaleValue = 1;
	TIM_Init(lpcTimer, TIM_TIMER_MODE, &lpcTimerConfig);
	lpcTimer->PR = 1000;

	TIM_Cmd(lpcTimer, FunctionalState::ENABLE);
}

void p_timer::reset() {
	lpcTimer->TC = 0;
}
uint32_t p_timer::getPrescaleTickRate() {
	return CLKPWR_GetPCLK(LpcTimerPClks[timerN]);
}
uint32_t p_timer::getBigTicks()	{
	return lpcTimer->TC;
}
uint32_t p_timer::getTicks()	{
	return lpcTimer->PC;
}
void p_timer::sleep(uint32_t duration) {
	uint32_t waitUntil = getBigTicks() + duration;
	while(getBigTicks() < waitUntil);
}

void
p_timer::setupRepeatingInterrupt(uint32_t interval)
{
	TIM_MATCHCFG_Type timerMatchConfig;
	timerMatchConfig.MatchChannel = 0;
	timerMatchConfig.MatchValue = interval;
	timerMatchConfig.IntOnMatch = ENABLE;
	timerMatchConfig.ResetOnMatch = ENABLE;
	timerMatchConfig.StopOnMatch = DISABLE;
	timerMatchConfig.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	TIM_ConfigMatch(lpcTimer, &timerMatchConfig);
	NVIC_EnableIRQ(LpcTimerIRQs[timerN]);
}

void
p_timer::setupCaptureInterrupt()
{
	lpcTimer->MCR = 0;
	lpcTimer->EMR = 0;

	TIM_CAPTURECFG_Type timerCapCfg;
	timerCapCfg.CaptureChannel = 0;
	timerCapCfg.FallingEdge = 1;
	timerCapCfg.RisingEdge = 1;
	timerCapCfg.IntOnCaption = 1;
	TIM_ConfigCapture(lpcTimer, &timerCapCfg);
	NVIC_EnableIRQ(LpcTimerIRQs[timerN]);
}
void
p_timer::clearInterrupt()
{
	lpcTimer->IR = 0xffffffff;
}
