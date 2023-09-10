#include <DriverTx.h>
#include "diag/trace.h"
#include "IEBusMessage.h"

DriverTx::DriverTx(p_timer timer)
	: DriverBase(timer),
	  state(&DriverTx::state_Idle)
{
	setTxPinState(false);
	pinConfigure(AVC_TX_PIN);
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

	bool bitVal = getNextBit();
	Time pulseTime = (T_Bit * (curBit-1)) + (bitVal ? T_Bit_1 : T_Bit_0);
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

	if (curBit == sendLengthBits) {
		messageDone();
		return;
	}

	Time pulseTime = T_Bit * curBit;
	timer.updateTimerAbsolute(startTime + pulseTime);

	state = &DriverTx::state_PeriodOff;
}

bool DriverTx::getNextBit() {
	bool bitVal;

	if(curField->isParity) {
		bitVal = curParity;
		curParity = false;
	}
	else if(curField->isAck) {
		bitVal = AckValue::NAK;
	}
	else {
		uint8_t whichBit = curField->bitOffset + curField->bitLength - curBit - 1;
		uint16_t* valuePtr = (uint16_t*)((uint8_t*)curMessage.get() + curField->valueOffset);
		bitVal = (*valuePtr) & (1UL<<whichBit);
		if(bitVal) {
			curParity = !curParity;
		}
	}

	curBit++;

	if(curBit == (curField->bitOffset + curField->bitLength)) {
		curField++;
	}

	return bitVal;
}

void DriverTx::messageDone() {
	state = &DriverTx::state_Idle;
	endTransmit();
}

void DriverTx::queueMessage(std::shared_ptr<IEBusMessage> message) {
	sendQueue.push(message);
}

bool DriverTx::isMessageWaiting() {
	return !(sendQueue.empty());
}

void DriverTx::prepareTransmit() {
	curMessage = sendQueue.front();
	sendLengthBits = curMessage->getMessageLength();
	sendQueue.pop();

	curField = IEBusFields.cbegin();
	curBit = 0;
	curParity = false;
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
