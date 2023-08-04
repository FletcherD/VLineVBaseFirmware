/*
 * AVCLanMsg.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: fletcher
 */
#include <AVCLanMsg.h>
#include "util.h"
#include <string>

constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::Broadcast;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::MasterAddress;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::MasterAddress_P;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::SlaveAddress;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::SlaveAddress_P;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::SlaveAddress_A;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::Control;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::Control_P;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::Control_A;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::DataLength;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::DataLength_P;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::DataLength_A;

AVCLanMsg::AVCLanMsg()
: messageBuf{0}
{
}

AVCLanMsg::AVCLanMsg(bool broadcast,
			uint16_t masterAddress,
			uint16_t slaveAddress,
			uint8_t control,
			std::vector<uint8_t> data)
: messageBuf{0}
{
	setField(AVCLanMsg::Broadcast, 			broadcast);
	setField(AVCLanMsg::MasterAddress, 		masterAddress);
	setField(AVCLanMsg::MasterAddress_P, 	calculateParity(masterAddress));
	setField(AVCLanMsg::SlaveAddress, 		slaveAddress);
	setField(AVCLanMsg::SlaveAddress_P, 	calculateParity(slaveAddress));
	setField(AVCLanMsg::SlaveAddress_A, 	AckValue::NAK);
	setField(AVCLanMsg::Control, 			control);
	setField(AVCLanMsg::Control_P, 			calculateParity(control));
	setField(AVCLanMsg::Control_A, 			AckValue::NAK);
	setField(AVCLanMsg::DataLength, 		data.size());
	setField(AVCLanMsg::DataLength_P, 		calculateParity(data.size()));
	setField(AVCLanMsg::DataLength_A, 		AckValue::NAK);
	for(uint8_t i = 0; i < data.size(); i++) {
		setField(AVCLanMsg::Data(i), 		data[i]);
		setField(AVCLanMsg::Data_P(i), 		calculateParity(data[i]));
		setField(AVCLanMsg::Data_A(i), 		AckValue::NAK);
	}
}

bool AVCLanMsg::getBit(uint32_t bitPos) {
	uint8_t whichByte = (bitPos / 8);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	whichByte = messageBufLen - whichByte - 1;
#endif
	uint8_t whichBit  = (bitPos % 8);
	return messageBuf[whichByte] & (0x80>>whichBit);
}

void AVCLanMsg::setBit(uint32_t bitPos, bool value) {
	uint8_t whichByte = (bitPos / 8);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	whichByte = messageBufLen - whichByte - 1;
#endif
	uint8_t whichBit  = (bitPos % 8);
	if(value) {
		messageBuf[whichByte] |= (0x80>>whichBit);
	} else {
		messageBuf[whichByte] &= ~(0x80>>whichBit);
	}
}

FieldValue AVCLanMsg::getField(AVCLanMsgField field)
{
	uint8_t lenBytes = sizeof(FieldValue);
	uint8_t startByte = field.BitOffset / 8;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	startByte = messageBufLen - startByte - lenBytes;
#endif
	FieldValue bitMask = (1UL<<field.LengthBits) - 1;
	uint8_t bitShift = (lenBytes*8) - field.LengthBits - (field.BitOffset%8);

	FieldValue value = *(FieldValue*)(messageBuf+startByte);
	value = (value >> bitShift) & bitMask;
	return value;
}

void AVCLanMsg::setField(AVCLanMsgField field, FieldValue value) {
	uint8_t lenBytes = sizeof(value);
	uint8_t startByte = field.BitOffset / 8;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	startByte = messageBufLen - startByte - lenBytes;
#endif
	FieldValue bitMask = (1UL<<field.LengthBits) - 1;
	uint8_t bitShift = (lenBytes*8) - field.LengthBits - (field.BitOffset%8);

	FieldValue* valuePtr = (uint32_t*)(messageBuf+startByte);
	*valuePtr &= ~(bitMask << bitShift);
	*valuePtr |= (value << bitShift);
}

uint32_t AVCLanMsg::getMessageLength()
{
	uint8_t dataLen = getField(DataLength);
	return AVCLanMsg::Data(0).BitOffset + dataLen*DataFieldLength;
}

bool AVCLanMsg::calculateParity(FieldValue data)
{
	bool parity = false;
	while(data != 0) {
		if(data & 1UL)
			parity = !parity;
		data = (data >> 1);
	}
	return parity;
}

bool AVCLanMsg::isAckBit(uint8_t bitPos)
{
	if(bitPos == AVCLanMsg::SlaveAddress_A.BitOffset)
		return true;
	if(bitPos == AVCLanMsg::Control_A.BitOffset)
		return true;
	if(bitPos == AVCLanMsg::DataLength_A.BitOffset)
		return true;
	if((bitPos >= AVCLanMsg::Data_A(0).BitOffset)
			&& (bitPos % DataFieldLength == AVCLanMsg::Data_A(0).BitOffset % DataFieldLength))
		return true;
	return false;
}

const char* AVCLanMsg::toString()
{
	bool broadcast 			= getField(AVCLanMsg::Broadcast);
	uint16_t masterAddress 	= getField(AVCLanMsg::MasterAddress);
	uint16_t slaveAddress 	= getField(AVCLanMsg::SlaveAddress);
	uint8_t control 		= getField(AVCLanMsg::Control);
	uint8_t dataLen 		= getField(AVCLanMsg::DataLength);

	char dataStr[dataLen*3+1];
	for(uint8_t i = 0; i < dataLen; i++) {
		snprintf(dataStr+i*3, 4, "%02x ", (unsigned int)getField(AVCLanMsg::Data(i)));
	}
	char* str;
	asiprintf(&str, "%c %03x %03x %c %01x %d \t%s\r\n",
			broadcast == AVCLanMsg::BROADCAST ? 'B' : '-',
			masterAddress, slaveAddress,
			getField(AVCLanMsg::SlaveAddress_A)==AVCLanMsg::ACK ? 'A' : 'a',
			control,
			dataLen,
			dataStr);
	return str;
}
