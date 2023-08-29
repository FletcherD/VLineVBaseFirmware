#include <DriverTx.h>

DriverTx::DriverTx(p_timer timer)
	: DriverBase(timer),
	  state(&DriverTx::state_Idle),
	  sendBitPos(0),
	  sendLengthBits(0),
	  ackResult(NAK)
{
	pinConfigure(AVC_TX_PIN);
	setTxPinState(false);

	timer.setupTimerInterrupt(T_Timeout);
}

void DriverTx::state_Idle() {
}

void DriverTx::state_StartBit() {
	startTime = timer.getTicks() + T_Bit_1;
	timer.updateTimerAbsolute(startTime);
	setTxPinState(false);
	state = &DriverTx::state_PeriodOff;
}

void DriverTx::state_PeriodOff() {
<<<<<<< HEAD
	if (sendBitPos == sendLengthBits) {
		setTxPinState(true);
		timer.updateTimer(T_Bit*4);
		state = &DriverTx::state_EndPause;
		return;
	}

=======
>>>>>>> 93eff14 (Interim state to allow reading of the code)
	setTxPinState(true);
	bool bit = sendQueue.front().getBit(sendBitPos);
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
	if(sendQueue.empty()) {
		setTxPinState(false);
		state = &DriverTx::state_Idle;
		endTransmit();
	} else {
		startTransmit();
	}
}

void DriverTx::queueMessage(MessageRaw message) {
	sendQueue.push(message);
}

bool DriverTx::isMessageWaiting() {
	return !(sendQueue.empty());
}

void DriverTx::startTransmit() {
	sendLengthBits = sendQueue.front().getMessageLength();
	sendBitPos = 0;

	timer.updateTimer(T_StartBit);
	setTxPinState(true);
	state = &DriverTx::state_StartBit;
}

void DriverTx::onTimerCallback() {
	(this->*state)();
}
<<<<<<< HEAD

void DriverTx::setTxPinState(bool isOn)	{
	// Active low
	gpioPinWrite(AVC_TX_PIN, !isOn);
}
=======
>>>>>>> 93eff14 (Interim state to allow reading of the code)
