#include <DriverRx.h>
#include <MessageRaw.h>
#include "diag/trace.h"

DriverRx::DriverRx(p_timer timer)
	: DriverBase(timer),
	  receiveBitPos(0)
{
	state = &DriverRx::state_Idle;

	pinConfigure(AVC_RX_PIN);
	pinConfigure(AVC_STB_PIN);
	pinConfigure(AVC_EN_PIN);
	setEnabled(true);
	setStandby(false);

	lastEventTime = timer.getTicks();
	timer.setupCaptureInterrupt();
	timer.setupTimerInterrupt(T_Timeout);
}

void DriverRx::onTimerCallback() {
	InputEvent event;
	uint32_t eventTime = timer.lpcTimer->CR0;
	uint32_t rxIn = GPIO_PortRead(AVC_RX_PIN.Portnum);
	event.time = eventTime - lastEventTime;
	event.type = (rxIn & (1UL << AVC_RX_PIN.Pinnum)) ? RISING_EDGE : FALLING_EDGE;
	lastEventTime = eventTime;

	uint32_t interruptType = timer.lpcTimer->IR;
	if((interruptType & 1UL) || event.time > T_Timeout)
	{
		state = &DriverRx::state_Idle;
		resetBuffer();
		endReceive();
	}

	(this->*state)(event);

	timer.updateTimer(T_Timeout);
}

void DriverRx::state_Idle(InputEvent e) {
	if(e.type == FALLING_EDGE) {
		state = &DriverRx::state_StartBit;
	}
}
void DriverRx::state_StartBit(InputEvent e) {
	if(e.type == RISING_EDGE) {
		if(e.time < (T_StartBit / 2)) {
			state = &DriverRx::state_Idle;
			return;
		}
		resetBuffer();
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
	for(size_t i = 0; i != MessageRaw::MaxMessageLenBytes; i++)
		thisMsg.messageBuf[i] = 0;
}

void DriverRx::receiveBit(bool bitVal) {
	thisMsg.setBit(receiveBitPos, bitVal);

	/* Because we have a very strict time budget,
	 * we calculate the expected message length on the fly
	 * so we know when we're done with minimal overhead
	 */
	if(receiveBitPos >= MessageRaw::DataLength.BitOffset)
	{
		uint8_t dataLenBitPos = (receiveBitPos - MessageRaw::DataLength.BitOffset);
		if(dataLenBitPos < MessageRaw::DataLength.LengthBits) {
			msgDataLength |= (bitVal << (MessageRaw::DataLength.LengthBits - dataLenBitPos - 1));
		} else {
			size_t thisMessageLength = MessageRaw::Data(0).BitOffset + (MessageRaw::DataFieldLength*msgDataLength);

			if((receiveBitPos+1) == thisMessageLength) {
				// Message is done
				state = &DriverRx::state_Idle;
				messageEnd();
			}
		}
	}

	receiveBitPos++;
}

void DriverRx::messageEnd() {
	if (receiveBitPos == 0)
		return;

	if(!thisMsg.isValid()) {
		char messageStr[256];
		char* pos = messageStr;
		for(size_t i = 0; i < MessageRaw::MaxMessageLenBytes; i++) {
			pos += sprintf(pos, "%02x ", thisMsg.messageBuf[i]);
		}
		trace_printf("Invalid message received: %s", messageStr);
		onBitError();
		return;
	}

	longestMsg = (receiveBitPos>longestMsg ? receiveBitPos : longestMsg);

	totalMsgCount++;
	//timer.setIrqEnabled(false);
	messageReceived(thisMsg);
	//timer.setIrqEnabled(true);
	resetBuffer();
	endReceive();
}

void DriverRx::onBitError() {
	bitErrorCount++;
	state = &DriverRx::state_Idle;
	resetBuffer();
	endReceive();
}
