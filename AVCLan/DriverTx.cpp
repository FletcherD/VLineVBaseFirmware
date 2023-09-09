#include <DriverTx.h>
#include "diag/trace.h"
#include "IEBusMessage.h"

DriverTx::DriverTx(p_timer timer)
	: DriverBase(timer),
	  state(&DriverTx::state_Idle),
	  curBit(0),
	  sendLengthBits(0),
	  ackResult(NAK)
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

	bool bit = getBit();
	Time pulseTime = (T_Bit * curBit) + (bit ? T_Bit_1 : T_Bit_0);
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
	curBit++;

	if (curBit == sendLengthBits) {
		messageDone();
		return;
	}

	Time pulseTime = T_Bit * curBit;
	timer.updateTimerAbsolute(startTime + pulseTime);

	state = &DriverTx::state_PeriodOff;
}

void DriverTx::state_GetAck() {
	uint32_t rxIn = GPIO_PortRead(AVC_RX_PIN.Portnum);
	rxIn = (rxIn & (1UL << AVC_RX_PIN.Pinnum));
	ackResult = (rxIn ? NAK : ACK);

	curBit++;
	Time pulseTime = T_Bit * curBit;
	timer.updateTimerAbsolute(startTime + pulseTime);
	state = &DriverTx::state_PeriodOff;
}

bool DriverTx::getBit() {
	bool bitVal;
	int32_t fieldBitPos = (int32_t)curBit - curField->bitOffset;

	if(curField->isParity) {
		bitVal = parity;
		parity = false;
	}
	else if(curField->isAck) {
		bitVal = AckValue::NAK;
	}
	else {
		uint8_t whichBit = (curField->bitLength - fieldBitPos - 1);
		uint16_t* valuePtr = (uint16_t*)(curMessage.get() + curField->valueOffset);
		bitVal = (*valuePtr) & (1UL<<whichBit);
		if(bitVal)
			parity = !parity;
	}

	curBit++;

	if(fieldBitPos == curField->bitLength) {
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
	parity = false;
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
