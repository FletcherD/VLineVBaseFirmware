#include "AVCLanTx.h"

AVCLanTx::AVCLanTx(p_timer timer)
	: AVCLanDrvBase(timer),
	  state(&AVCLanTx::state_Idle),
	  sendBitPos(0),
	  sendLengthBits(0),
	  ackResult(AVCLanMsg::NAK)
{
	pinConfigure(AVC_TX_PIN);
	setTxPinState(false);

	timer.setupTimerInterrupt(T_Timeout);
}

void AVCLanTx::state_Idle() {
}

void AVCLanTx::state_StartBit() {
	startTime = timer.getTicks() + T_Bit_1;
	timer.updateTimerAbsolute(startTime);
	setTxPinState(false);
	state = &AVCLanTx::state_PeriodOff;
}

void AVCLanTx::state_PeriodOff() {
	if (sendBitPos == sendLengthBits) {
		messageDone();
		return;
	}

	setTxPinState(true);
	bool bit = sendQueue.front().getBit(sendBitPos);
	Time pulseTime = (T_Bit * sendBitPos) + (bit ? T_Bit_1 : T_Bit_0);
	timer.updateTimerAbsolute(startTime + pulseTime);
	state = &AVCLanTx::state_PeriodOn;
}

void AVCLanTx::state_PeriodOn() {
	setTxPinState(false);

	/*
	if(sendBitPos == AVCLanMsg::SlaveAddress_A.BitOffset) {
		Time pulseTime = (T_Bit * sendBitPos) + T_BitMeasure;
		timer.updateTimerAbsolute(startTime + pulseTime);
		state = &AVCLanTx::state_GetAck;
		return;
	}
	*/

	sendBitPos++;
	Time pulseTime = T_Bit * sendBitPos;
	timer.updateTimerAbsolute(startTime + pulseTime);
	state = &AVCLanTx::state_PeriodOff;
}

void AVCLanTx::state_GetAck() {
	uint32_t rxIn = GPIO_PortRead(AVC_RX_PIN.Portnum);
	rxIn = (rxIn & (1UL << AVC_RX_PIN.Pinnum));
	ackResult = (rxIn ? AVCLanMsg::NAK : AVCLanMsg::ACK);

	sendBitPos++;
	Time pulseTime = T_Bit * sendBitPos;
	timer.updateTimerAbsolute(startTime + pulseTime);
	state = &AVCLanTx::state_PeriodOff;
}

void AVCLanTx::messageDone() {
	sendQueue.pop();
	if(sendQueue.empty()) {
		state = &AVCLanTx::state_Idle;
		endTransmit();
	} else {
		startTransmit();
	}
}

void AVCLanTx::queueMessage(AVCLanMsg message) {
	sendQueue.push(message);
}

bool AVCLanTx::isMessageWaiting() {
	return !(sendQueue.empty());
}

void AVCLanTx::startTransmit() {
	sendLengthBits = sendQueue.front().getMessageLength();
	sendBitPos = 0;

	timer.updateTimer(T_StartBit);
	setTxPinState(true);
	state = &AVCLanTx::state_StartBit;
}

void AVCLanTx::onTimerCallback() {
	(this->*state)();
}

void AVCLanTx::setTxPinState(bool isOn)	{
	// Active low
	gpioPinWrite(AVC_TX_PIN, !isOn);
}
