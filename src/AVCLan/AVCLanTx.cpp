#include "AVCLanTx.h"

AVCLanTx::AVCLanTx(p_timer timer)
	: AVCLanDrvBase(timer),
	  state(&AVCLanTx::state_Idle),
	  sendBitPos(0),
	  sendLengthBits(0)
{
	pinConfigure(AVC_TX_PIN);
	setTxPinState(false);

	timer.setupTimerInterrupt(T_Timeout);
}

void AVCLanTx::state_Idle(SendEvent i) {
	if(i == SEND_REQUESTED) {
		startTransmit();
	}
}

void AVCLanTx::state_StartBit(SendEvent i) {
	if(i == TIMER_TRIGGERED) {
		timer.updateTimer(T_Bit_0);
		setTxPinState(false);
		state = &AVCLanTx::state_PeriodOff;
	}
}

void AVCLanTx::state_PeriodOff(SendEvent i) {
	if(i == TIMER_TRIGGERED) {
		bool bit = sendQueue.front().getBit(sendBitPos);
		Time pulseDur;
		if (bit)
			pulseDur = T_Bit_1;
		else
			pulseDur = T_Bit_0;

		timer.updateTimer(pulseDur);
		setTxPinState(true);
		state = &AVCLanTx::state_PeriodOn;
	}
}

void AVCLanTx::state_PeriodOn(SendEvent i) {
	if(i == TIMER_TRIGGERED) {
		bool bit = sendQueue.front().getBit(sendBitPos);
		Time pulseDur;
		if (bit)
			pulseDur = T_Bit - T_Bit_1;
		else
			pulseDur = T_Bit - T_Bit_0;

		timer.updateTimer(pulseDur);
		setTxPinState(false);

		sendBitPos++;
		if (sendBitPos == sendLengthBits) {
			// Message done
			sendQueue.pop();
			timer.updateTimer(T_EndWait);
			state = &AVCLanTx::state_EndWait;
		} else {
			state = &AVCLanTx::state_PeriodOff;
		}
	}
}

void AVCLanTx::state_EndWait(SendEvent i) {
	if(i == TIMER_TRIGGERED) {
		if(sendQueue.empty()) {
			state = &AVCLanTx::state_Idle;
			endTransmit();
		} else {
			startTransmit();
		}
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
	(this->*state)(TIMER_TRIGGERED);
}

void AVCLanTx::setTxPinState(bool isOn)	{
	// Active low
	gpioPinWrite(AVC_TX_PIN, !isOn);

	sendX[sendI] = timer.getTicks();
	sendY[sendI] = isOn;
	sendI++;
	if(sendI == 256) {
		for(sendI=0; sendI!=256; sendI++) {
			uartOut.printf("%d %d\r\n", sendX[sendI], sendY[sendI]);
		}
		sendI=0;
	}
}
