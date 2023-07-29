/*
 * AVCLanMsg.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: fletcher
 */
#include <AVCLanMsg.h>
#include "util.h"

constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::Broadcast;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::MasterAddress;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::SlaveAddress;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::SlaveAddress_P;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::Control;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::Control_P;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::DataLength;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::DataLength_P;

AVCLanMsg::AVCLanMsg()
: messageBuf{0},
  currentBit(0)
{
}

AVCLanMsg::AVCLanMsg(bool broadcast,
			uint16_t masterAddress,
			uint16_t slaveAddress,
			uint8_t control,
			std::vector<uint8_t> data)
: messageBuf{0},
  currentBit(0)
{
}

bool AVCLanMsg::getNextBit() {
	uint8_t fieldBit = currentBit - currentField.BitOffset;
	if(fieldBit == currentField.LengthBits) {
		currentField = nextField(currentField);
		fieldBit = currentBit - currentField.BitOffset;
	}

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
	static constexpr uint32_t dataFieldLength =
			AVCLanMsg::Data(0).LengthBits +
			AVCLanMsg::Data_P(0).LengthBits +
			AVCLanMsg::Data_A(0).LengthBits;
	static constexpr uint32_t dataFieldOffset = AVCLanMsg::Data(0).BitOffset;
	uint8_t dataLen = getField(DataLength);
	return dataFieldOffset + dataLen*dataFieldLength;
}

