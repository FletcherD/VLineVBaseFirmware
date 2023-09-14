#include <DriverTx.h>
#include "diag/trace.h"
#include "IEBusMessage.h"

DriverTx::DriverTx(p_timer timer)
	: DriverBase(timer),
	  state(&DriverTx::state_Idle)
{
	setTxState(false);
	pinConfigure(AvcTxPin);
}

void DriverTx::state_Idle() {
}

void DriverTx::state_StartBit() {
	setTxState(false);

	startTime = timer.getTicks() + T_Bit_1;
	timer.updateTimerAbsolute(startTime);

	state = &DriverTx::state_PeriodOff;
}

void DriverTx::state_PeriodOff() {
	setTxState(true);

	curBitValue = getBit();
	Time pulseTime = (T_Bit * curBit) + (curBitValue ? T_Bit_1 : T_Bit_0);
	timer.updateTimerAbsolute(startTime + pulseTime);

	state = &DriverTx::state_PeriodOn;
}

void DriverTx::state_PeriodOn() {
	setTxState(false);
	/*
	if(sendBitPos == AVCLanMsg::SlaveAddress_A.BitOffset) {
		Time pulseTime = (T_Bit * sendBitPos) + T_BitMeasure;
		timer.updateTimerAbsolute(startTime + pulseTime);
		state = &AVCLanTx::state_GetAck;
		return;
	}
	*/

	if(curBitValue) {
		Time checkTime = (T_Bit * curBit) + T_BitMeasure;
		timer.updateTimerAbsolute(startTime + checkTime);
		state = &DriverTx::state_CheckCollision;
	} else {
		nextBit();
		checkMessageDone();

		Time pulseTime = T_Bit * curBit;
		timer.updateTimerAbsolute(startTime + pulseTime);
		state = &DriverTx::state_PeriodOff;
	}
}

void DriverTx::state_CheckCollision() {
	bool lineState = getRxState();
	if(!lineState) {
		// Someone else has set the line
		if(curField->isAck) {
			// We've been ACKed
		} else {
			// There's been a collision
			// We were never sending a message... we were receiving a message the whole time!
			startReceive();
			collisionRecover();
			return;
		}
	}

	nextBit();
	checkMessageDone();

	Time pulseTime = T_Bit * curBit;
	timer.updateTimerAbsolute(startTime + pulseTime);
	state = &DriverTx::state_PeriodOff;
}

bool DriverTx::getBit() {
	bool bitVal;

	if(curField->isParity) {
		bitVal = curParity;
	}
	else if(curField->isAck) {
		bitVal = AckValue::NAK;
	}
	else {
		uint8_t whichBit = curField->bitOffset + curField->bitLength - curBit - 1;
		auto* valuePtr = (uint16_t*)((uint8_t*)curMessage.get() + curField->valueOffset);
		bitVal = (*valuePtr) & (1UL<<whichBit);

		if(bitVal) {
			curParity = !curParity;
		}
	}

	return bitVal;
}

void DriverTx::checkMessageDone() {
	if (curBit == sendLengthBits) {
		// Message sent successfully
		sendQueue.pop();
		messageDone();
		return;
	}
}

void DriverTx::messageDone() {
	state = &DriverTx::state_Idle;
	endTransmit();
}

void DriverTx::queueMessage(const std::shared_ptr<IEBusMessage>& message) {
	sendQueue.push(message);
}

bool DriverTx::isMessageWaiting() {
	return !(sendQueue.empty());
}

void DriverTx::prepareTransmit() {
	curMessage.reset(new IEBusMessage(*sendQueue.front()));
	sendLengthBits = curMessage->getMessageLength();

	curField = IEBusFields.cbegin();
	curBit = 0;
	curParity = false;
}

void DriverTx::startTransmit() {
	setTxState(true);
	startTime = timer.getTicks() + T_StartBit;
	timer.updateTimerAbsolute(startTime);
	state = &DriverTx::state_StartBit;
}

void DriverTx::onTimerCallback() {
	(this->*state)();
}
