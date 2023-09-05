#include <DriverTx.h>

#include "diag/trace.h"

DriverTx::DriverTx(p_timer timer)
	: DriverBase(timer),
	  state(&DriverTx::state_Idle),
	  sendBitPos(0),
	  sendLengthBits(0),
	  ackResult(NAK)
{
	setTxPinState(false);
	pinConfigure(AVC_TX_PIN);

	//timer.setupTimerInterrupt(T_Timeout);
}

void DriverTx::state_Idle() {
}

void DriverTx::state_StartBit() {
	setTxPinState(false);
	startTime = timer.getTicks() + T_Bit_1;
	timer.updateTimerAbsolute(startTime);
	state = &DriverTx::state_PeriodOff;
}

void DriverTx::state_PeriodOff() {
	setTxPinState(true);
	bool bit = curMessage->getBit(sendBitPos);
	Time pulseTime = (T_Bit * sendBitPos) + (bit ? T_Bit_1 : T_Bit_0);
	timer.updateTimerAbsolute(startTime + pulseTime);
	state = &DriverTx::state_PeriodOn;
}

void DriverTx::state_PeriodOn() {
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

	if (sendBitPos == sendLengthBits) {
		// TODO: why often get bit error on RX after sending message?
		messageDone();
		return;
	}

	Time pulseTime = T_Bit * sendBitPos;
	timer.updateTimerAbsolute(startTime + pulseTime);
	state = &DriverTx::state_PeriodOff;
}

void DriverTx::state_EndPause() {

}

void DriverTx::state_GetAck() {
	uint32_t rxIn = GPIO_PortRead(AVC_RX_PIN.Portnum);
	rxIn = (rxIn & (1UL << AVC_RX_PIN.Pinnum));
	ackResult = (rxIn ? NAK : ACK);

	sendBitPos++;
	Time pulseTime = T_Bit * sendBitPos;
	timer.updateTimerAbsolute(startTime + pulseTime);
	state = &DriverTx::state_PeriodOff;
}

void DriverTx::messageDone() {
	sendQueue.pop();
	state = &DriverTx::state_Idle;
	endTransmit();
}

void DriverTx::queueMessage(MessageRawPtr message) {
	sendQueue.push(message);
}

bool DriverTx::isMessageWaiting() {
	return !(sendQueue.empty());
}

void DriverTx::prepareTransmit() {
	curMessage = sendQueue.front();

	char messageStr[256];
	curMessage->toString(messageStr);
	trace_printf("Sending message: %s", messageStr);

	sendLengthBits = curMessage->getMessageLength();
	sendBitPos = 0;
}

void DriverTx::startTransmit() {
	setTxPinState(true);
	startTime = timer.getTicks() + T_StartBit;
	timer.updateTimerAbsolute(startTime);
	state = &DriverTx::state_StartBit;
}

void DriverTx::onTimerCallback() {
	(this->*state)();
}
