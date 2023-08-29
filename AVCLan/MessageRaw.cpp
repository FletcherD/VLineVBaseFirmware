/*
 * AVCLanMsg.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: fletcher
 */
#include <MessageRaw.h>
#include "util.h"
#include <string>
#include <cstring>

constexpr MessageRaw::MessageField MessageRaw::Broadcast;
constexpr MessageRaw::MessageField MessageRaw::MasterAddress;
constexpr MessageRaw::MessageField MessageRaw::MasterAddress_P;
constexpr MessageRaw::MessageField MessageRaw::SlaveAddress;
constexpr MessageRaw::MessageField MessageRaw::SlaveAddress_P;
constexpr MessageRaw::MessageField MessageRaw::SlaveAddress_A;
constexpr MessageRaw::MessageField MessageRaw::Control;
constexpr MessageRaw::MessageField MessageRaw::Control_P;
constexpr MessageRaw::MessageField MessageRaw::Control_A;
constexpr MessageRaw::MessageField MessageRaw::DataLength;
constexpr MessageRaw::MessageField MessageRaw::DataLength_P;
constexpr MessageRaw::MessageField MessageRaw::DataLength_A;

MessageRaw::MessageRaw()
: messageBuf{0}
{
}

MessageRaw::MessageRaw(const MessageRaw& otherMessage)
{
	memcpy(messageBuf, otherMessage.messageBuf, MaxMessageLenBytes);
}
MessageRaw::MessageRaw(const uint8_t* otherBuf)
{
	memcpy(messageBuf, otherBuf, MaxMessageLenBytes);
}

MessageRaw::MessageRaw(bool broadcast,
			uint16_t masterAddress,
			uint16_t slaveAddress,
			uint8_t control,
			std::vector<uint8_t> data)
: messageBuf{0}
{
	setField(MessageRaw::Broadcast, 			broadcast);
	setField(MessageRaw::MasterAddress, 		masterAddress);
	setField(MessageRaw::MasterAddress_P, 	calculateParity(masterAddress));
	setField(MessageRaw::SlaveAddress, 		slaveAddress);
	setField(MessageRaw::SlaveAddress_P, 	calculateParity(slaveAddress));
	setField(MessageRaw::SlaveAddress_A, 	AckValue::NAK);
	setField(MessageRaw::Control, 			control);
	setField(MessageRaw::Control_P, 			calculateParity(control));
	setField(MessageRaw::Control_A, 			AckValue::NAK);
	setField(MessageRaw::DataLength, 		data.size());
	setField(MessageRaw::DataLength_P, 		calculateParity(data.size()));
	setField(MessageRaw::DataLength_A, 		AckValue::NAK);
	for(uint8_t i = 0; i < data.size(); i++) {
		setField(MessageRaw::Data(i), 		data[i]);
		setField(MessageRaw::Data_P(i), 		calculateParity(data[i]));
		setField(MessageRaw::Data_A(i), 		AckValue::NAK);
	}
}

bool MessageRaw::getBit(uint32_t bitPos) const
{
	uint8_t whichByte = (bitPos / 8);
<<<<<<< HEAD
	/*
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	whichByte = MaxMessageLenBytes - whichByte - 1;
#endif
*/
=======
>>>>>>> 93eff14 (Interim state to allow reading of the code)
	uint8_t whichBit  = (bitPos % 8);
	return messageBuf[whichByte] & (0x80>>whichBit);
}

void MessageRaw::setBit(uint32_t bitPos, bool value)
{
	uint8_t whichByte = (bitPos / 8);
<<<<<<< HEAD
	/*
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	whichByte = MaxMessageLenBytes - whichByte - 1;
#endif
*/
	uint8_t whichBit  = (bitPos % 8);
	if(value) {
		messageBuf[whichByte] |= (0x80>>whichBit);
=======
	uint8_t whichBit  = (bitPos % 8);
	if(value) {
		messageBuf[whichByte] |=  (0x80>>whichBit);
>>>>>>> 93eff14 (Interim state to allow reading of the code)
	} else {
		messageBuf[whichByte] &= ~(0x80>>whichBit);
	}
}

FieldValue MessageRaw::getField(MessageField field) const
{
	uint8_t lenBytes = sizeof(FieldValue);
	uint8_t startByte = field.BitOffset / 8;
	FieldValue bitMask = (1UL<<field.LengthBits) - 1;
	uint8_t bitShift = (lenBytes*8) - field.LengthBits - (field.BitOffset%8);

	FieldValue value = *(FieldValue*)(messageBuf+startByte);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	value = swapBytes(value);
#endif
<<<<<<< HEAD
=======

>>>>>>> 93eff14 (Interim state to allow reading of the code)
	value = (value >> bitShift) & bitMask;
	return value;
}

void MessageRaw::setField(MessageField field, FieldValue value)
{
	uint8_t lenBytes = sizeof(value);
	uint8_t startByte = field.BitOffset / 8;
	FieldValue bitMask = (1UL<<field.LengthBits) - 1;
	uint8_t bitShift = (lenBytes*8) - field.LengthBits - (field.BitOffset%8);

<<<<<<< HEAD
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	swapBytes(value);
#endif

	bitMask = bitMask << bitShift;
	value = value << bitShift;

	FieldValue* valuePtr = (uint32_t*)(messageBuf+startByte);
=======
	bitMask = bitMask << bitShift;
	value = value << bitShift;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	value = swapBytes(value);
#endif

	FieldValue* valuePtr = (FieldValue*)(messageBuf+startByte);
>>>>>>> 93eff14 (Interim state to allow reading of the code)
	*valuePtr &= ~(bitMask);
	*valuePtr |= (value);
}

uint32_t MessageRaw::getMessageLength() const
{
	uint8_t dataLen = getField(DataLength);
	return MessageRaw::Data(0).BitOffset + dataLen*DataFieldLength;
}

bool MessageRaw::calculateParity(FieldValue data)
{
	bool parity = false;
	while(data != 0) {
		if(data & 1UL)
			parity = !parity;
		data = (data >> 1);
	}
	return parity;
}

bool MessageRaw::isAckBit(uint8_t bitPos)
{
	if(bitPos == MessageRaw::SlaveAddress_A.BitOffset)
		return true;
	if(bitPos == MessageRaw::Control_A.BitOffset)
		return true;
	if(bitPos == MessageRaw::DataLength_A.BitOffset)
		return true;
	if((bitPos >= MessageRaw::Data_A(0).BitOffset)
			&& (bitPos % DataFieldLength == MessageRaw::Data_A(0).BitOffset % DataFieldLength))
		return true;
	return false;
}

size_t MessageRaw::toString(char* str) const
{
	uint8_t dataLen = getField(MessageRaw::DataLength);

	char* pos = str;
<<<<<<< HEAD
	pos += sprintf(pos, "%c %03x %03x %01x %d \t",
=======
	pos += sprintf(pos, "%c %03x %03x %01x %d : ",
>>>>>>> 93eff14 (Interim state to allow reading of the code)
		(getField(MessageRaw::Broadcast)==BROADCAST ? 'B' : '-'),
		getField(MessageRaw::MasterAddress),
		getField(MessageRaw::SlaveAddress),
		getField(MessageRaw::Control),
		dataLen);
	// TODO Prevent overflow
	//dataLen = (dataLen>21 ? 21 : dataLen);
	for(uint8_t i = 0; i != dataLen; i++) {
		pos += sprintf(pos, "%02x ", getField(MessageRaw::Data(i)));
	}
	return (pos-str);
}

bool MessageRaw::isValid() const
{
	if(getMessageLength() > (MaxMessageLenBytes*8))
		return false;
	if(calculateParity(getField(MasterAddress)) != getField(MasterAddress_P))
		return false;
	if(calculateParity(getField(SlaveAddress)) != getField(SlaveAddress_P))
		return false;
	if(calculateParity(getField(Control)) != getField(Control_P))
		return false;
	uint8_t dataLen = getField(DataLength);
	if(calculateParity(dataLen) != getField(DataLength_P))
		return false;
	for(size_t i = 0; i != dataLen; i++) {
		if(calculateParity(getField(Data(i))) != getField(Data_P(i)))
			return false;
	}
	return true;
}
<<<<<<< HEAD
=======


>>>>>>> 93eff14 (Interim state to allow reading of the code)
