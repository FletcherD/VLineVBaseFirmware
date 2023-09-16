/*
 * IEBusMessage.h
 *
 *  Created on: Sep 8, 2023
 *      Author: fletcher
 */

#ifndef IEBUSMESSAGE_H_
#define IEBUSMESSAGE_H_

#include "cmsis_device.h"
#include "util.h"

#include <array>
#include <memory>

typedef enum {
    UNICAST   = 1,
    BROADCAST = 0
} BroadcastValue;
typedef enum {
    NAK = 1,
    ACK = 0
} AckValue;
typedef uint8_t ControlValue;
typedef uint8_t DataValue;
typedef uint16_t Address;

struct IEBusMessage {
	static constexpr size_t MaxDataBytes = 32;

	BroadcastValue broadcast	= BROADCAST;
	Address masterAddress		= 0;
	Address slaveAddress		= 0;
	ControlValue control		= 0xf;
	uint8_t dataLength			= 0;
	DataValue data[MaxDataBytes] {0};

	uint32_t getMessageLength() const;
};

struct IEBusMessageField {
	uint32_t 	bitOffset;
	uint8_t 	bitLength;
	bool		isAck		=false;
	bool 		isParity	=false;
	uint32_t	valueOffset	=0;
};

constexpr std::array IEBusHeaderFields{
	IEBusMessageField{ .bitOffset = 0,	.bitLength = 1,		.valueOffset 	= offsetof(IEBusMessage, broadcast) },
	IEBusMessageField{ .bitOffset = 1,	.bitLength = 12,	.valueOffset 	= offsetof(IEBusMessage, masterAddress) },
	IEBusMessageField{ .bitOffset = 13,	.bitLength = 1,		.isParity 		= true },
	IEBusMessageField{ .bitOffset = 14,	.bitLength = 12,	.valueOffset 	= offsetof(IEBusMessage, slaveAddress) },
	IEBusMessageField{ .bitOffset = 26,	.bitLength = 1,		.isParity 		= true },
	IEBusMessageField{ .bitOffset = 27,	.bitLength = 1,		.isAck 			= true },
	IEBusMessageField{ .bitOffset = 28,	.bitLength = 4,		.valueOffset 	= offsetof(IEBusMessage, control) },
	IEBusMessageField{ .bitOffset = 32,	.bitLength = 1,		.isParity 		= true },
	IEBusMessageField{ .bitOffset = 33,	.bitLength = 1,		.isAck 			= true },
	IEBusMessageField{ .bitOffset = 34,	.bitLength = 8,		.valueOffset 	= offsetof(IEBusMessage, dataLength) },
	IEBusMessageField{ .bitOffset = 42,	.bitLength = 1,		.isParity 		= true },
	IEBusMessageField{ .bitOffset = 43,	.bitLength = 1,		.isAck 			= true },
};

// 8 data, 1 parity, 1 ack
constexpr size_t DataFieldLength = 8+1+1;

constexpr IEBusMessageField IEBusDataField(uint8_t N) {
	return IEBusMessageField{ .bitOffset = 44 + (N*DataFieldLength), .bitLength = 8, .valueOffset 	= offsetof(IEBusMessage, data)+N };
}
constexpr IEBusMessageField IEBusDataField_P(uint8_t N) {
	return IEBusMessageField{ .bitOffset = 44+8+(N*DataFieldLength), .bitLength = 1, .isParity 		= true };
}
constexpr IEBusMessageField IEBusDataField_A(uint8_t N) {
	return IEBusMessageField{ .bitOffset = 44+9+(N*DataFieldLength), .bitLength = 1, .isAck 		= true };
}

constexpr auto initIEBusFields() {
	constexpr size_t size = IEBusHeaderFields.size() + (IEBusMessage::MaxDataBytes*3);
	std::array<IEBusMessageField, size> IEBusFields{};
	size_t i = 0;
	while(i < IEBusHeaderFields.size()) {
		IEBusFields[i] = IEBusHeaderFields[i];
		i++;
	}
	for(size_t di = 0; di < IEBusMessage::MaxDataBytes; di++) {
		IEBusFields[i++] = IEBusDataField(di);
		IEBusFields[i++] = IEBusDataField_P(di);
		IEBusFields[i++] = IEBusDataField_A(di);
	}
	return IEBusFields;
}

constexpr std::array IEBusFields = initIEBusFields();

#endif /* IEBUSMESSAGE_H_ */
