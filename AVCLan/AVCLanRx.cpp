#include <AVCLanMsg.h>
#include "AVCLanRx.h"
#include "diag/trace.h"

AVCLanRx::AVCLanRx(p_timer timer)
	: AVCLanDrvBase(timer),
	  receiveBitPos(0)
{
	state = &AVCLanRx::state_Idle;

	pinConfigure(AVC_RX_PIN);
	pinConfigure(AVC_STB_PIN);
	pinConfigure(AVC_EN_PIN);
	setEnabled(true);
	setStandby(false);

	lastEventTime = timer.getTicks();
	timer.setupCaptureInterrupt();
	timer.setupTimerInterrupt(T_Timeout);
}

void AVCLanRx::onTimerCallback() {
	InputEvent event;
	uint32_t eventTime = timer.lpcTimer->CR0;
	uint32_t rxIn = GPIO_PortRead(AVC_RX_PIN.Portnum);
	event.time = eventTime - lastEventTime;
	event.type = (rxIn & (1UL << AVC_RX_PIN.Pinnum)) ? RISING_EDGE : FALLING_EDGE;
	lastEventTime = eventTime;

	uint32_t interruptType = timer.lpcTimer->IR;
	if((interruptType & 1UL) || event.time > T_Timeout)
	{
		state = &AVCLanRx::state_Idle;
		messageEnd();
	}

	(this->*state)(event);

	timer.updateTimer(T_Timeout);
}

void AVCLanRx::state_Idle(InputEvent e) {
	if(e.type == FALLING_EDGE) {
		state = &AVCLanRx::state_StartBit;
	}
}
void AVCLanRx::state_StartBit(InputEvent e) {
	if(e.type == RISING_EDGE) {
		if(e.time < (T_StartBit / 2)) {
			state = &AVCLanRx::state_Idle;
			return;
		}
		resetBuffer();
		state = &AVCLanRx::state_WaitForBit;
	}
}
void AVCLanRx::state_WaitForBit(InputEvent e) {
	if(e.type != FALLING_EDGE || e.time > T_Bit) {
		onBitError();
		return;
	}
	state = &AVCLanRx::state_MeasureBit;
}
void AVCLanRx::state_MeasureBit(InputEvent e) {
	if(e.type != RISING_EDGE || e.time > T_Bit) {
		onBitError();
		return;
	}
	state = &AVCLanRx::state_WaitForBit;
	bool bitVal = (e.time > T_BitMeasure) ? false : true;
	receiveBit(bitVal);
}

void AVCLanRx::resetBuffer() {
	receiveBitPos = 0;
}

void AVCLanRx::receiveBit(bool bitVal) {
	thisMsg.setBit(receiveBitPos, bitVal);
	receiveBitPos++;

	if (receiveBitPos >= AVCLanMsg::Data(0).BitOffset
		&& receiveBitPos == thisMsg.getMessageLength())
	{
		// Message is done
		state = &AVCLanRx::state_Idle;
		messageEnd();
	}
}

void AVCLanRx::messageEnd() {
	if (receiveBitPos == 0)
		return;

	if(!thisMsg.isValid() || receiveBitPos != thisMsg.getMessageLength()) {
		onBitError();
		return;
	}

	resetBuffer();
	timer.setIrqEnabled(false);
	messageReceived(thisMsg);
	timer.setIrqEnabled(true);
}

void AVCLanRx::onBitError() {
	bitErrorCount++;
	state = &AVCLanRx::state_Idle;
	resetBuffer();
	endReceive();
}
