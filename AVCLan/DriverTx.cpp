#include <DriverTx.h>

#include <memory>
#include "diag/trace.h"
#include "IEBusMessage.h"

DriverTx::DriverTx(p_timer& timer)
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

	if(curBitValue) {
		Time checkTime = (T_Bit * curBit) + T_BitMeasure;
		timer.updateTimerAbsolute(startTime + checkTime);
		state = &DriverTx::state_CheckCollision;
	} else {
		nextBit();
		if(!checkMessageDone()) {
			Time pulseTime = T_Bit * curBit;
			timer.updateTimerAbsolute(startTime + pulseTime);
			state = &DriverTx::state_PeriodOff;
		}
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
			collisionCount++;
			curMessage.reset(new IEBusMessage(curSendMessage));
			sendQueue.push(std::make_shared<IEBusMessage>(curSendMessage));
			startReceive();
			collisionRecover();
			return;
		}
	}

	nextBit();
	if(!checkMessageDone()) {
		Time pulseTime = T_Bit * curBit;
		timer.updateTimerAbsolute(startTime + pulseTime);
		state = &DriverTx::state_PeriodOff;
	}
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
		auto* valuePtr = (uint16_t*)((uint8_t*)&curSendMessage + curField->valueOffset);
		bitVal = (*valuePtr) & (1UL<<whichBit);

		if(bitVal) {
			curParity = !curParity;
		}
	}

	return bitVal;
}

bool DriverTx::checkMessageDone() {
	if (curBit == sendLengthBits) {
		// Message sent successfully
		messageDone();
		return true;
	}
	return false;
}

void DriverTx::messageDone() {
	totalMsgsSent++;
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
	curSendMessage = IEBusMessage(*sendQueue.front());
	sendQueue.pop();
	sendLengthBits = curSendMessage.getMessageLength();

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
