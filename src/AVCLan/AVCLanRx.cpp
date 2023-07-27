#include "AVCLanRx.h"
#include "AVCLanMsg.h"
#include "diag/trace.h"

void AVCLanRx::onTimerCallback() {
	uint32_t interruptType = timer.lpcTimer->IR;
	timer.clearInterrupt();

	InputEvent event;
	uint32_t eventTime = timer.lpcTimer->CR0;
	uint32_t rxIn = GPIO_PortRead(AVC_RX_PIN.Portnum);
	event.time = eventTime - lastEventTime;
	event.type = (rxIn & (1UL << AVC_RX_PIN.Pinnum)) ? RISING_EDGE : FALLING_EDGE;
	lastEventTime = eventTime;

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
		if(e.time < 100) {
			// Spurious
			state = &AVCLanRx::state_Idle;
			return;
		}
		resetBuffer();
		state = &AVCLanRx::state_WaitForBit;
	}
}
void AVCLanRx::state_WaitForBit(InputEvent e) {
	if(e.type == FALLING_EDGE) {
		state = &AVCLanRx::state_MeasureBit;
	}
}
void AVCLanRx::state_MeasureBit(InputEvent e) {
	if(e.type == RISING_EDGE) {
		state = &AVCLanRx::state_WaitForBit;

		if(e.time > T_Bit) {
			resetBuffer();
			state = &AVCLanRx::state_Idle;
			uartOut.printf("\tAbort\r\n");
			return;
		}

		bool bitVal = (e.time > T_BitMeasure) ? false : true;
		receiveBit(bitVal);
	}
}

void AVCLanRx::resetBuffer() {
	bufPos = 0;
}

void AVCLanRx::receiveBit(bool bit) {
	uint8_t whichByte = (bufPos / 8);
	uint8_t whichBit  = (bufPos % 8);
	if (whichBit==0) {
		messageBuf[whichByte] = 0;
	}
	if (bit) {
		messageBuf[whichByte] |= (0x80>>whichBit);
	}
	bufPos++;

	if (bufPos >= AVCLanMsg::Data(0).BitOffset
		&& bufPos == thisMsg.getMessageLength())
	{
		// Message is done
		state = &AVCLanRx::state_Idle;
		messageEnd();
	}
}

void AVCLanRx::messageEnd() {
	if (bufPos == 0)
		return;

	timer.setIrqEnabled(false);

	bool broadcast 			= thisMsg.getField(AVCLanMsg::Broadcast);
	uint16_t masterAddress 	= thisMsg.getField(AVCLanMsg::MasterAddress);
	uint16_t slaveAddress 	= thisMsg.getField(AVCLanMsg::SlaveAddress);
	uint8_t control 		= thisMsg.getField(AVCLanMsg::Control);
	uint8_t dataLen 		= thisMsg.getField(AVCLanMsg::DataLength);
	uint8_t data[dataLen];
	for(uint8_t i = 0; i < dataLen; i++) {
		data[i] 			= thisMsg.getField(AVCLanMsg::Data(i));
	}

	char dataStr[dataLen*3+1];
	for(uint8_t i = 0; i < dataLen; i++) {
		snprintf(dataStr+i*3, 4, "%02x ", data[i]);
	}
	uartOut.printf("%c %03x %03x %01x %d \t%s\r\n",
			(broadcast == AVCLanMsg::AVC_MSG_BROADCAST ? 'B' : '-'),
			masterAddress, slaveAddress, control, dataLen, dataStr);

	resetBuffer();
	timer.setIrqEnabled(true);
}

AVCLanRx::AVCLanRx(p_timer timer)
	: AVCLanDrvBase(timer),
	messageBuf{0},
	thisMsg(messageBuf)
{
	state = &AVCLanRx::state_Idle;

	pinConfigure(AVC_RX_PIN);
	//pinConfigure(AVC_TX_PIN);
	pinConfigure(AVC_STB_PIN);
	pinConfigure(AVC_EN_PIN);
	//pinConfigure(AVC_ERR_PIN);

	setEnabled(true);
	setStandby(false);
	//setTx(false);

	lastEventTime = timer.getTicks();
	timer.setupCaptureInterrupt();
	timer.setupTimerInterrupt(T_Timeout);
}

