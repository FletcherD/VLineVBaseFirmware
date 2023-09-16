#include <DriverRx.h>
#include <iterator>
#include "diag/trace.h"
#include "IEBusMessage.h"

DriverRx::DriverRx(p_timer& timer)
	: DriverBase(timer)
{
	resetBuffer();
	state = &DriverRx::state_Idle;

	setEnabled(true);
	setStandby(false);
	pinConfigure(AvcEnablePin);
	pinConfigure(AvcRxPin);
	pinConfigure(AvcStandbyPin);

	lastEventTime = timer.getTicks();
	timer.setupCaptureInterrupt();
}

void DriverRx::onTimerCallback() {
	uint32_t interruptType = timer.lpcTimer->IR;
	uint32_t eventTime =
			(interruptType & (1UL<<TIM_CR0_INT) ? timer.lpcTimer->CR0 : timer.lpcTimer->MR0);

	InputEvent event{};
	event.time = eventTime - lastEventTime;
	event.type = getRxState() ? RISING_EDGE : FALLING_EDGE;
	lastEventTime = eventTime;

	(this->*state)(event);
}

void DriverRx::state_Idle(InputEvent e) {
	if(e.type == FALLING_EDGE) {
		//resetBuffer();
		state = &DriverRx::state_StartBit;
	} else {
		endReceive();
		//trace_printf("state_Idle %d T: %d Bit: %d", e.type, e.time, curBit);
	}
}

void DriverRx::state_StartBit(InputEvent e) {
	if(e.type == RISING_EDGE) {
		if(e.time < (T_StartBit / 2) || e.time > (T_StartBit * 2)) {
			// Not really a start bit
			//trace_printf("%d", e.time);
			state = &DriverRx::state_Idle;
			endReceive();
			return;
		}

		state = &DriverRx::state_WaitForBit;
	} else {
		//trace_printf("state_StartBit %d T: %d Bit: %d", e.type, e.time, curBit);
		endReceive();
	}
}

void DriverRx::state_WaitForBit(InputEvent e) {
	if(e.type != FALLING_EDGE || e.time > T_Bit) {
		//trace_printf("state_WaitForBit %d T: %d Bit: %d", e.type, e.time, curBit);
		onBitError();
		return;
	}

	state = &DriverRx::state_MeasureBit;
}

void DriverRx::state_MeasureBit(InputEvent e) {
	if(e.type != RISING_EDGE || e.time > T_Bit) {
		//trace_printf("state_MeasureBit %d T: %d Bit: %d", e.type, e.time, curBit);
		onBitError();
		return;
	}
	//state = &DriverRx::state_WaitForBit;
	curBitValue = (e.time <= T_BitMeasure);
	receiveBit(curBitValue);

	nextBit();

	TIM_CAPTURECFG_Type timerCapCfg;
	timerCapCfg.CaptureChannel = 0;
	timerCapCfg.FallingEdge = 0;
	timerCapCfg.RisingEdge = 1;
	timerCapCfg.IntOnCaption = 1;
	TIM_ConfigCapture(timer.lpcTimer, &timerCapCfg);

	state = &DriverRx::state_MeasureBit_Rising;

	//checkMessageDone();
}

void DriverRx::state_MeasureBit_Rising(InputEvent e) {
	if(e.time > T_RisingMeasure_0) {
		curBitValue = false;
	} else if (e.time < T_RisingMeasure_1) {
		curBitValue = true;
	}

	state = &DriverRx::state_MeasureBit_Rising;

	receiveBit(curBitValue);
	nextBit();

	if(curMessage->slaveAddress == 0x1d3 && curField->isAck) {
		timer.setCaptureInterruptEnabled(false);
		timer.setTimerInterruptEnabled(true);

		const Time bitEndTime = lastEventTime + (curBitValue ? (T_Bit - T_Bit_1) : (T_Bit - T_Bit_0));
		const Time ackTime = bitEndTime + T_Ack;
		timer.updateTimerAbsolute(ackTime);

		state = &DriverRx::state_WaitToAck;
	}

	checkMessageDone();
}

void DriverRx::state_WaitToAck(InputEvent i) {
	setTxState(true);
	curBitValue = false;

	const Time ackEndTime = lastEventTime + (T_Bit - T_Bit_0) + T_Ack;
	timer.updateTimerAbsolute(ackEndTime);

	state = &DriverRx::state_Ack;
}

void DriverRx::state_Ack(InputEvent e) {
	setTxState(false);

	timer.setCaptureInterruptEnabled(true);
	timer.setTimerInterruptEnabled(false);

	TIM_CAPTURECFG_Type timerCapCfg;
	timerCapCfg.CaptureChannel = 0;
	timerCapCfg.FallingEdge = 0;
	timerCapCfg.RisingEdge = 1;
	timerCapCfg.IntOnCaption = 1;
	TIM_ConfigCapture(timer.lpcTimer, &timerCapCfg);

	state = &DriverRx::state_MeasureBit_Rising;
	nextBit();
	checkMessageDone();
}

void DriverRx::collisionRecover() {
	state = &DriverRx::state_WaitForBit;
	receiveBit(false);
	checkMessageDone();
}

void DriverRx::receiveBit(bool bitVal) {
	if(curField->isParity) {
		if(bitVal != curParity) {
			// TODO: react to incorrect parity
		}
	}
	else if(curField->isAck) {
	}
	else {
		uint8_t whichBit = curField->bitOffset + curField->bitLength - curBit - 1;
		auto* valuePtr = (uint16_t*)((uint8_t*)curMessage.get() + curField->valueOffset);
		if(bitVal) {
			*valuePtr |=  (1UL<<whichBit);
			curParity = !curParity;
		} else {
			*valuePtr &= ~(1UL<<whichBit);
		}
	}
}

void DriverRx::checkMessageDone() {
	if(curBit >= IEBusDataField(0).bitOffset) {
		uint32_t thisMessageLength = IEBusDataField(0).bitOffset + (curMessage->dataLength * DataFieldLength);
		if(curBit == thisMessageLength) {
			// Message is done
			messageDone();
			return;
		}
	}
}

void DriverRx::messageDone() {
	longestMsg = (curBit>longestMsg ? curBit : longestMsg);

	totalMsgCount++;
	messageReceived(curMessage);

	state = &DriverRx::state_Idle;
	resetBuffer();

	TIM_CAPTURECFG_Type timerCapCfg;
	timerCapCfg.CaptureChannel = 0;
	timerCapCfg.FallingEdge = 1;
	timerCapCfg.RisingEdge = 1;
	timerCapCfg.IntOnCaption = 1;
	TIM_ConfigCapture(timer.lpcTimer, &timerCapCfg);

	endReceive();
}

void DriverRx::onBitError() {
	bitErrorCount++;

	state = &DriverRx::state_Idle;
	resetBuffer();
	endReceive();
}
