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
		resetBuffer();
		endReceive();
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
		//trace_printf("%d %d", e.time, e.type);
		onBitError();
		return;
	}
	state = &AVCLanRx::state_MeasureBit;
}
void AVCLanRx::state_MeasureBit(InputEvent e) {
	if(e.type != RISING_EDGE || e.time > T_Bit) {
		//trace_printf("%d %d", e.time, e.type);
		onBitError();
		return;
	}
	state = &AVCLanRx::state_WaitForBit;
	bool bitVal = (e.time > T_BitMeasure) ? false : true;
	receiveBit(bitVal);
}

void AVCLanRx::resetBuffer() {
	receiveBitPos = 0;
	msgDataLength = 0;
	for(size_t i = 0; i != AVCLanMsg::MaxMessageLenBytes; i++)
		thisMsg.messageBuf[i] = 0;
}

void AVCLanRx::receiveBit(bool bitVal) {
	thisMsg.setBit(receiveBitPos, bitVal);

	/* Because we have a very strict time budget,
	 * we calculate the expected message length on the fly
	 * so we know when we're done with minimal overhead
	 */
	if(receiveBitPos >= AVCLanMsg::DataLength.BitOffset)
	{
		uint8_t dataLenBitPos = (receiveBitPos - AVCLanMsg::DataLength.BitOffset);
		if(dataLenBitPos < AVCLanMsg::DataLength.LengthBits) {
			msgDataLength |= (bitVal << (AVCLanMsg::DataLength.LengthBits - dataLenBitPos - 1));
		} else {
			size_t thisMessageLength = AVCLanMsg::Data(0).BitOffset + (AVCLanMsg::DataFieldLength*msgDataLength);

			if((receiveBitPos+1) == thisMessageLength) {
				// Message is done
				state = &AVCLanRx::state_Idle;
				messageEnd();
			}
		}
	}

	receiveBitPos++;
}

void AVCLanRx::messageEnd() {
	if (receiveBitPos == 0)
		return;

	if(!thisMsg.isValid()) {
		char messageStr[256];
		char* pos = messageStr;
		for(size_t i = 0; i < AVCLanMsg::MaxMessageLenBytes; i++) {
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

void AVCLanRx::onBitError() {
	bitErrorCount++;
	state = &AVCLanRx::state_Idle;
	resetBuffer();
	endReceive();
}
