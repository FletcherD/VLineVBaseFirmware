#include "AVCLanDrv.h"
#include "diag/trace.h"

extern "C" {
void TIMER2_IRQHandler(void) {
	AVCLanDrv::instance->onTimerCallback();
}
}

void AVCLanDrv::onTimerCallback() {
	uint32_t interruptType = timer.lpcTimer->IR;
	timer.clearInterrupt();
	timer.updateTimer(T_Timeout);

	InputEvent event;
	uint32_t eventTime = timer.lpcTimer->CR0;
	uint32_t rxIn = GPIO_PortRead(AVC_RX_PIN.Portnum);
	event.time = eventTime - lastEventTime;
	event.type = (rxIn & (1UL << AVC_RX_PIN.Pinnum)) ? RISING_EDGE : FALLING_EDGE;
	lastEventTime = eventTime;

	SendData thisData;
	thisData.time = event.time;
	thisData.data = event.type;

	sendBuf[sendPos++] = thisData;

	if(sendPos == sendBufSize) {
		timer.setIrqEnabled(false);
		for (uint i = 0; i != sendBufSize; i++) {
			uartOut.printf("%d\t%d\r\n", sendBuf[i].time, sendBuf[i].data);
		}
		sendPos = 0;
		timer.setIrqEnabled(true);
	}

	//uartOut.printf("%d\t%d\t%d \r\n", eventTime, event.time, event.type);
/*
	if((interruptType & 1UL) || event.time > T_Timeout)
	{
		if(state != &AVCLanDrv::state_Idle)
			uartOut.printf("Timeout %d\n", timer.lpcTimer->TC);
		state = &AVCLanDrv::state_Idle;
		messageEnd();
	}
	(this->*state)(event);
*/
}


void AVCLanDrv::state_Idle(InputEvent e) {
	if(e.type == FALLING_EDGE) {
		state = &AVCLanDrv::state_StartBit;
	}
}

void AVCLanDrv::state_StartBit(InputEvent e) {
	if(e.type == RISING_EDGE) {
		if(e.time < 10) {
			// Spurious
			state = &AVCLanDrv::state_Idle;
			return;
		}
		uartOut.printf("StartBitLen: %d\r\n", e.time);
		bufPos = 0;
		state = &AVCLanDrv::state_WaitForBit;
	}
}
void AVCLanDrv::state_WaitForBit(InputEvent e) {
	if(e.type == FALLING_EDGE) {
		state = &AVCLanDrv::state_MeasureBit;
	}
}
void AVCLanDrv::state_MeasureBit(InputEvent e) {
	if(e.type == RISING_EDGE) {
		uartOut.printf("BitLen: %d\r\n", e.time);
		state = &AVCLanDrv::state_WaitForBit;

		bool bitVal = (e.time > T_BitMeasure) ? false : true;
		receiveBit(bitVal);
	}
}

void AVCLanDrv::receiveBit(bool bit) {
	if(bit) {
		uartOut.printf("ReceiveBit: %d \r\n", bit?1:0);
		uint8_t whichByte = (bufPos / 8);
		uint8_t whichBit  = (bufPos % 8);
		messageBuf[whichByte] |= (1<<whichBit);
	}
	bufPos++;
}

void AVCLanDrv::messageEnd() {
	uartOut.printf("MessageEnd ");
	uint8_t messageLen = bufPos/8 + 1;
	for (uint8_t i = 0; i != messageLen; i++) {
		uartOut.printf("%02x ", messageBuf[i]);
		messageBuf[i] = 0;
	}
	uartOut.printf("\r\n");
	bufPos = 0;
}

// AVCLan driver & timer2 init,
void AVCLanDrv::begin () {
	AVCLanDrv::instance = this;

	state = &AVCLanDrv::state_Idle;

	pinConfigure(AVC_RX_PIN);
	pinConfigure(AVC_TX_PIN);
	pinConfigure(AVC_STB_PIN);
	pinConfigure(AVC_EN_PIN);
	pinConfigure(AVC_ERR_PIN);

	pinConfigure(AVC_EN2_PIN);
	setEnabled2(false);

	timer.setupCaptureInterrupt();
	//timer.setupTimerInterrupt(T_Timeout);

	setStandby(false);
	setEnabled(true);
	setTx(false);
}

AVCLanDrv* AVCLanDrv::instance;
