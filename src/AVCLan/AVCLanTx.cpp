#include "AVCLanTx.h"

AVCLanTx::AVCLanTx(p_timer timer)
	: AVCLanDrvBase(timer),
	  state(&AVCLanTx::state_Idle)
{
}

void AVCLanTx::state_Idle(SendEvent i) {
	if(i == SEND_REQUESTED) {
		startTransmit();
	}
}

void AVCLanTx::state_StartBit(SendEvent i) {
	if(i == TIMER_TRIGGERED) {
		timer.setupTimerInterrupt(T_Bit_0);
		setTxPinState(false);
		state = &AVCLanTx::state_PeriodOff;
	}
}

void AVCLanTx::state_PeriodOff(SendEvent i) {
	if(i == TIMER_TRIGGERED) {
		bool bit = getNextBit();
		Time pulseDur;
		if (bit)
			pulseDur = T_Bit_1;
		else
			pulseDur = T_Bit_0;

		timer.setupTimerInterrupt(pulseDur);
		setTxPinState(true);
		state = &AVCLanTx::state_PeriodOn;
	}
}

void AVCLanTx::state_PeriodOn(SendEvent i) {
	if(i == TIMER_TRIGGERED) {
		bool bit = getNextBit();
		Time pulseDur;
		if (bit)
			pulseDur = T_Bit - T_Bit_1;
		else
			pulseDur = T_Bit - T_Bit_0;

		timer.setupTimerInterrupt(pulseDur);
		setTxPinState(false);

		bufPos++;
		if (bufPos == sendQueue.front().lengthBits) {
			// Message done
			sendQueue.pop();
			timer.setupTimerInterrupt(T_EndWait);
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
	timer.setupTimerInterrupt(T_StartBit);
	setTxPinState(true);
	state = &AVCLanTx::state_StartBit;
}

void AVCLanTx::onTimerCallback() {
	(this->*state)(TIMER_TRIGGERED);
}

bool AVCLanTx::getNextBit() {
	uint8_t* messageBuf = sendQueue.front().messageBuf;
	uint8_t whichByte = (bufPos / 8);
	uint8_t whichBit  = (bufPos % 8);
	bool bit = messageBuf[whichByte] & (0x80>>whichBit);
	return bit;
}

void AVCLanTx::setTxPinState(bool isOn)	{
	// Active low
	gpioPinWrite(AVC_TX_PIN, !isOn);
}
