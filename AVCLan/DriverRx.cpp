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

	uint32_t eventTime;
	uint32_t interruptType = timer.lpcTimer->IR;
	if(interruptType & (1UL<<TIM_CR0_INT)) {
		eventTime = timer.lpcTimer->CR0;
	} else {
		eventTime = timer.lpcTimer->MR0;
	}
	event.time = eventTime - lastEventTime;
	event.type = getRxPinState() ? RISING_EDGE : FALLING_EDGE;
	lastEventTime = eventTime;

	(this->*state)(event);

	//timer.updateTimer(T_Timeout);
}

void DriverRx::state_Idle(InputEvent e) {
	if(e.type == FALLING_EDGE) {
		//resetBuffer();
		state = &DriverRx::state_StartBit;
	} else {
		endReceive();
		trace_printf("state_Idle %d %d", e.type, e.time);
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
		trace_printf("state_StartBit %d %d", e.type, e.time);
		endReceive();
	}
}

void DriverRx::state_WaitForBit(InputEvent e) {
	if(e.time > T_Bit) {
		trace_printf("state_WaitForBit %d %d", e.type, e.time);
		onBitError();
		return;
	}

	if(msgSlaveAddress == 0x1d3 && MessageRaw::isAckBit(receiveBitPos)) {
		setTxPinState(true);

		timer.setCaptureInterruptEnabled(false);
		timer.setTimerInterruptEnabled(true);

		Time ackEndTime = lastEventTime + T_Bit_0;
		timer.updateTimerAbsolute(ackEndTime);

		state = &DriverRx::state_Ack;
	} else {
		state = &DriverRx::state_MeasureBit;
	}
}

void DriverRx::state_MeasureBit(InputEvent e) {
	if(e.time > T_Bit) {
		trace_printf("state_MeasureBit %d %d", e.type, e.time);
		onBitError();
		return;
	}
	state = &DriverRx::state_WaitForBit;
	bool bitVal = (e.time > T_BitMeasure) ? false : true;
	receiveBit(bitVal);

	receiveBitPos++;
}

void DriverRx::state_Ack(InputEvent e) {
	setTxPinState(false);

	timer.setCaptureInterruptEnabled(true);
	timer.setTimerInterruptEnabled(false);

	state = &DriverRx::state_WaitForBit;

	receiveBitPos++;
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
	if(receiveBitToValue(bitVal, MessageRaw::DataLength, (FieldValue*)&msgDataLength)) {
		uint32_t thisMessageLength = MessageRaw::Data(0).BitOffset + (MessageRaw::DataFieldLength*msgDataLength);
		if(receiveBitPos == thisMessageLength) {
			// Message is done
			messageEnd();
			return;
		}
	}

	receiveBitToValue(bitVal, MessageRaw::SlaveAddress, (FieldValue*)&msgSlaveAddress);
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
	uint8_t test[] = {1,2,3,4,5,6,7,8};
	uint8_t& testR = test[4];

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

bool DriverRx::receiveBitToValue(bool bitVal, MessageRaw::MessageField field, FieldValue* valuePtr) {

	int32_t bitPos = (int32_t)receiveBitPos - field.BitOffset;
	if(bitPos >= 0)
	{
		if(bitPos < field.LengthBits) {
			if(bitVal) {
				uint8_t whichBit = (field.LengthBits - bitPos - 1);
				*valuePtr |= (1UL<<whichBit);
			}
			if(bitPos+1 == field.LengthBits) {
				return true;
			}
		} else {
			return true;
		}
	}
	return false;
}
