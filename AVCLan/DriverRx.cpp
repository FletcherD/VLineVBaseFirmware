#include <DriverRx.h>
#include <MessageRaw.h>
#include "diag/trace.h"

DriverRx::DriverRx(p_timer timer)
	: DriverBase(timer)
{
	resetBuffer();
	state = &DriverRx::state_Idle;

	setEnabled(true);
	setStandby(false);
	pinConfigure(AVC_EN_PIN);
	pinConfigure(AVC_RX_PIN);
	pinConfigure(AVC_STB_PIN);

	lastEventTime = timer.getTicks();
	timer.setupCaptureInterrupt();
	//timer.setupTimerInterrupt(T_Timeout);
}

void DriverRx::onTimerCallback() {
	InputEvent event;
	uint32_t eventTime = timer.lpcTimer->CR0;
	event.time = eventTime - lastEventTime;
	event.type = getRxPinState() ? RISING_EDGE : FALLING_EDGE;
	lastEventTime = eventTime;

	/*
	uint32_t interruptType = timer.lpcTimer->IR;
	if(event.time > T_Timeout)
	{
		trace_printf("Timeout %d %d", event.time, event.type);
		state = &DriverRx::state_Idle;
		resetBuffer();
	}
	*/

	(this->*state)(event);

	//timer.updateTimer(T_Timeout);
}

void DriverRx::state_Idle(InputEvent e) {
	if(e.type == FALLING_EDGE) {
		//resetBuffer();
		state = &DriverRx::state_StartBit;
	}
}
void DriverRx::state_StartBit(InputEvent e) {
	if(e.type == RISING_EDGE) {
		if(e.time < (T_StartBit / 2) || e.time > (T_StartBit * 2)) {
			// Not really a start bit
			state = &DriverRx::state_Idle;
			return;
		}
		state = &DriverRx::state_WaitForBit;
	}
}
void DriverRx::state_WaitForBit(InputEvent e) {
	if(e.type != FALLING_EDGE || e.time > T_Bit) {
		//trace_printf("%d %d", e.time, e.type);
		onBitError();
		return;
	}
	state = &DriverRx::state_MeasureBit;
}
void DriverRx::state_MeasureBit(InputEvent e) {
	if(e.type != RISING_EDGE || e.time > T_Bit) {
		//trace_printf("%d %d", e.time, e.type);
		onBitError();
		return;
	}
	state = &DriverRx::state_WaitForBit;
	bool bitVal = (e.time > T_BitMeasure) ? false : true;
	receiveBit(bitVal);
}

void DriverRx::resetBuffer() {
	receiveBitPos = 0;
	msgDataLength = 0;
	thisMsg.reset(new MessageRaw);
}

void DriverRx::receiveBit(bool bitVal) {
	thisMsg->setBit(receiveBitPos, bitVal);

	/* Because we have a very strict time budget,
	 * we calculate the expected message length on the fly
	 * so we know when we're done with minimal overhead
	 */
	int32_t dataLenBitPos = ((int32_t)receiveBitPos - MessageRaw::DataLength.BitOffset);
	receiveBitPos++;
	if(dataLenBitPos >= 0)
	{
		if(dataLenBitPos < MessageRaw::DataLength.LengthBits) {
			if(bitVal) {
				msgDataLength |= (0x80>>dataLenBitPos);
			}
		} else {
			uint32_t thisMessageLength = MessageRaw::Data(0).BitOffset + (MessageRaw::DataFieldLength*msgDataLength);

			if(receiveBitPos == thisMessageLength) {
				// Message is done
				messageEnd();
			}
		}
	}
}

void DriverRx::messageEnd() {

	/*
	if(!thisMsg->isValid()) {
		char messageStr[256];
		trace_printf("Invalid message received: ", messageStr);
		char* pos = messageStr;
		for(size_t i = 0; i < MessageRaw::MaxMessageLenBytes; i++) {
			pos += sprintf(pos, "%02x ", thisMsg->messageBuf[i]);
			if(i%32 == 0) {
				trace_printf("%s", messageStr);
				pos = messageStr;
			}
		}
		onBitError();
		return;
	}
	*/

	longestMsg = (receiveBitPos>longestMsg ? receiveBitPos : longestMsg);

	totalMsgCount++;
	//timer.setIrqEnabled(false);
	messageReceived(thisMsg);
	//timer.setIrqEnabled(true);

	state = &DriverRx::state_Idle;
	resetBuffer();
	endReceive();
}

void DriverRx::onBitError() {
	bitErrorCount++;

	state = &DriverRx::state_Idle;
	resetBuffer();
	endReceive();
}
