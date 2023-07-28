#include "AVCLanTx.h"

AVCLanTx::AVCLanTx(p_timer timer)
	: AVCLanDrvBase(timer)
{

}

void AVCLanTx::state_Idle(SendEvent i) {
	if(i == SEND_REQUESTED) {
		setTx(true);
		timer.setupTimerInterrupt(T_StartBit);

		state = &AVCLanTx::state_StartBit;
	}
}

void AVCLanTx::state_StartBit(SendEvent i) {
	if(i == TIMER_TRIGGERED) {
		setTx(false);
		timer.setupTimerInterrupt(T_Bit_0);

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

		setTx(true);
		timer.setupTimerInterrupt(pulseDur);

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

		setTx(false);
		timer.setupTimerInterrupt(pulseDur);

		bufPos++;
		if (bufPos == sendQueue.front().size) {
			sendQueue.pop();
			state = &AVCLanTx::state_Idle;
		} else {
			state = &AVCLanTx::state_PeriodOff;
		}
	}
}

void AVCLanTx::endTransmission() {

}

void AVCLanTx::queueMessage(uint8_t* thisMessage, uint32_t thisMessageLengthBits) {
	uint8_t thisMessageLengthBytes = (thisMessageLengthBits/8) + (thisMessageLengthBits%8 ? 1 : 0);

	SendData thisData;
	thisData.data = new uint8_t[thisMessageLengthBytes];
	for (uint8_t i = 0; i < thisMessageLengthBytes; i++) {
		thisData.data[i] = thisMessage[i];
	}
	thisData.size = thisMessageLengthBits;
	sendQueue.push(thisData);
}

bool AVCLanTx::isBusy() {
	return (sendQueue.empty());
}

void AVCLanTx::onTimerCallback() {
	(this->*state)(TIMER_TRIGGERED);
}

bool AVCLanTx::getNextBit() {
	uint8_t* messageBuf = sendQueue.front().data;
	uint8_t whichByte = (bufPos / 8);
	uint8_t whichBit  = (bufPos % 8);
	bool bit = messageBuf[whichByte] & (0x80>>whichBit);
	return bit;
}
