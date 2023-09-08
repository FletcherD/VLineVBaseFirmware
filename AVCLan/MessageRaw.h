/*
 * AVCLanMsg.h
 *
 *  Created on: Jul 24, 2023
 *      Author: fletcher
 */
#include "cmsis_device.h"
#include "util.h"

#include <vector>
#include <map>
#include <string>
#include <memory>

#ifndef AVCLAN_AVCLANMSG_H_
#define AVCLAN_AVCLANMSG_H_

typedef uint32_t FieldValue;

typedef enum {
    UNICAST    = 1,
    BROADCAST = 0
} BroadcastValue;
typedef enum {
    NAK = 1,
    ACK = 0
} AckValue;
typedef uint8_t ControlValue;
typedef uint8_t DataValue;
typedef uint8_t Opcode;
typedef uint8_t Function;
typedef uint16_t Address;

class MessageRaw {
	/*
	 * This is a raw AVC-LAN message.
	 * The message is stored as a raw buffer of bytes.
	 * This is needed for speed purposes when sending or receiving;
	 * we have a very strict time budget when Rx or Tx is in progress.
	 * Fields in the message can be get or set
	 * using the getField() or setField() methods.
	 */
public:

	static constexpr uint8_t MaxMessageLenBytes = 64;
	uint8_t messageBuf[MaxMessageLenBytes];

	// Info about Message Fields
	struct MessageField {
		uint32_t 	BitOffset;
		uint8_t 	LengthBits;
		bool		IsAck;
	};

	static constexpr MessageField Broadcast 		= {BitOffset: 0,	LengthBits: 1,	IsAck: false };
	static constexpr MessageField MasterAddress 	= {BitOffset: 1,	LengthBits: 12,	IsAck: false };
	static constexpr MessageField MasterAddress_P 	= {BitOffset: 13,	LengthBits: 1,	IsAck: false };
	static constexpr MessageField SlaveAddress	 	= {BitOffset: 14,	LengthBits: 12,	IsAck: false };
	static constexpr MessageField SlaveAddress_P 	= {BitOffset: 26,	LengthBits: 1,	IsAck: false };
	static constexpr MessageField SlaveAddress_A 	= {BitOffset: 27,	LengthBits: 1,	IsAck: true };
	static constexpr MessageField Control 			= {BitOffset: 28,	LengthBits: 4,	IsAck: false };
	static constexpr MessageField Control_P 		= {BitOffset: 32,	LengthBits: 1,	IsAck: false };
	static constexpr MessageField Control_A 		= {BitOffset: 33, 	LengthBits: 1,	IsAck: true	};
	static constexpr MessageField DataLength 		= {BitOffset: 34,	LengthBits: 8,	IsAck: false };
	static constexpr MessageField DataLength_P 		= {BitOffset: 42,	LengthBits: 1,	IsAck: false };
	static constexpr MessageField DataLength_A 		= {BitOffset: 43,	LengthBits: 1,	IsAck: true	};
	static constexpr uint8_t DataFieldLength = 10;
	static constexpr MessageField Data(uint8_t N) {
		return MessageField( { BitOffset:	44U + (DataFieldLength*N), LengthBits: 8, IsAck: false } );
	}
	static constexpr MessageField Data_P(uint8_t N) {
		return MessageField( { BitOffset:	44U+8+(DataFieldLength*N), LengthBits: 1, IsAck: false } );
	}
	static constexpr MessageField Data_A(uint8_t N) {
		return MessageField( { BitOffset:	44U+9+(DataFieldLength*N), LengthBits: 1, IsAck: true } );
	}

	// ---------------------------

	MessageRaw();
	MessageRaw(const MessageRaw&);
	MessageRaw(const uint8_t*);
	MessageRaw(bool broadcast,
			Address masterAddress,
			Address slaveAddress,
			ControlValue control,
			std::vector<DataValue> data);

	bool getBit(uint32_t bitPos) const;
	void setBit(uint32_t bitPos, bool value);

	FieldValue getField(MessageField field) const;
	void setField(MessageField field, FieldValue value);

	static bool isAckBit(uint32_t bitPos);

	static bool	calculateParity(FieldValue data);

	uint32_t getMessageLength() const;
	bool isValid() const;
	size_t toString(char*) const;
};

typedef std::shared_ptr<MessageRaw> MessageRawPtr;

struct Message {
	BroadcastValue broadcast;
	Address masterAddress;
	Address slaveAddress;
	ControlValue control;
	Function srcFunction;
	Function dstFunction;
	DataValue opcode;
	std::vector<DataValue> operands;

	size_t toString(char* str) const;
};

#endif /* AVCLAN_AVCLANMSG_H_ */
