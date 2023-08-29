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
typedef uint8_t LogicalDevice;
typedef uint16_t Address;

class MessageRaw {
public:
	struct MessageField {
		uint8_t 	BitOffset;
		uint8_t 	LengthBits;
		bool		IsAck;
		inline bool operator==(const MessageField& other) const
			{ return BitOffset == other.BitOffset; }
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
		return MessageField( { BitOffset:	44 + (DataFieldLength*N), LengthBits: 8, IsAck: false } );
	}
	static constexpr MessageField Data_P(uint8_t N) {
		return MessageField( { BitOffset:	44+8+(DataFieldLength*N), LengthBits: 1, IsAck: false } );
	}
	static constexpr MessageField Data_A(uint8_t N) {
		return MessageField( { BitOffset:	44+9+(DataFieldLength*N), LengthBits: 1, IsAck: true } );
	}

	static constexpr uint8_t MaxMessageLenBytes = 64;
	uint8_t messageBuf[MaxMessageLenBytes];

	MessageRaw();
	MessageRaw(const MessageRaw&);
	MessageRaw(const uint8_t*);
	MessageRaw(bool broadcast,
			uint16_t masterAddress,
			uint16_t slaveAddress,
			uint8_t control,
			std::vector<uint8_t> data);

	bool getBit(uint32_t bitPos) const;
	void setBit(uint32_t bitPos, bool value);

	FieldValue getField(MessageField field) const;
	void setField(MessageField field, FieldValue value);

	static bool isAckBit(uint8_t bitPos);

	static bool	calculateParity(FieldValue data);

	uint32_t getMessageLength() const;
	bool isValid() const;
	size_t toString(char*) const;
};

struct Message {
	BroadcastValue broadcast;
	Address masterAddress;
	Address slaveAddress;
	ControlValue control;
	LogicalDevice srcDevice;
	LogicalDevice dstDevice;
	DataValue opcode;
	std::vector<DataValue> operands;
};

#endif /* AVCLAN_AVCLANMSG_H_ */
