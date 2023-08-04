/*
 * AVCLanMsg.h
 *
 *  Created on: Jul 24, 2023
 *      Author: fletcher
 */
#include "cmsis_device.h"
#include "util.h"

#include <vector>
#include <string>

#ifndef AVCLAN_AVCLANMSG_H_
#define AVCLAN_AVCLANMSG_H_

typedef uint32_t FieldValue;

class AVCLanMsg {
public:
	struct AVCLanMsgField {
		uint8_t 	BitOffset;
		uint8_t 	LengthBits;
		bool		IsAck;
		inline bool operator==(const AVCLanMsgField& other) const
			{ return BitOffset == other.BitOffset; }
	};

	typedef enum {
	    DIRECT    = 1,
	    BROADCAST = 0
	} BroadcastValue;
	typedef enum {
	    NAK = 1,
	    ACK = 0
	} AckValue;

	static constexpr AVCLanMsgField Broadcast 			= {BitOffset: 0,	LengthBits: 1,	IsAck: false };
	static constexpr AVCLanMsgField MasterAddress 		= {BitOffset: 1,	LengthBits: 12,	IsAck: false };
	static constexpr AVCLanMsgField MasterAddress_P 	= {BitOffset: 13,	LengthBits: 1,	IsAck: false };
	static constexpr AVCLanMsgField SlaveAddress	 	= {BitOffset: 14,	LengthBits: 12,	IsAck: false };
	static constexpr AVCLanMsgField SlaveAddress_P 		= {BitOffset: 26,	LengthBits: 1,	IsAck: false };
	static constexpr AVCLanMsgField SlaveAddress_A 		= {BitOffset: 27,	LengthBits: 1,	IsAck: true };
	static constexpr AVCLanMsgField Control 			= {BitOffset: 28,	LengthBits: 4,	IsAck: false };
	static constexpr AVCLanMsgField Control_P 			= {BitOffset: 32,	LengthBits: 1,	IsAck: false };
	static constexpr AVCLanMsgField Control_A 			= {BitOffset: 33, 	LengthBits: 1,	IsAck: true	};
	static constexpr AVCLanMsgField DataLength 			= {BitOffset: 34,	LengthBits: 8,	IsAck: false };
	static constexpr AVCLanMsgField DataLength_P 		= {BitOffset: 42,	LengthBits: 1,	IsAck: false };
	static constexpr AVCLanMsgField DataLength_A 		= {BitOffset: 43,	LengthBits: 1,	IsAck: true	};
	static constexpr uint8_t DataFieldLength = 10;
	static constexpr AVCLanMsgField Data(uint8_t N) {
		return AVCLanMsgField( { BitOffset: 44+  (DataFieldLength*N), LengthBits: 8, IsAck: false } );
	}
	static constexpr AVCLanMsgField Data_P(uint8_t N) {
		return AVCLanMsgField( { BitOffset:	44+8+(DataFieldLength*N), LengthBits: 1, IsAck: false } );
	}
	static constexpr AVCLanMsgField Data_A(uint8_t N) {
		return AVCLanMsgField( { BitOffset:	44+9+(DataFieldLength*N), LengthBits: 1, IsAck: true } );
	}

	static constexpr uint8_t MaxMessageLenBytes = 32;
	static constexpr uint8_t messageBufLen = MaxMessageLenBytes+sizeof(FieldValue);
	uint8_t messageBuf[messageBufLen];

	AVCLanMsg();
	AVCLanMsg(bool broadcast,
			uint16_t masterAddress,
			uint16_t slaveAddress,
			uint8_t control,
			std::vector<uint8_t> data);

	bool getBit(uint32_t bitPos);
	void setBit(uint32_t bitPos, bool value);

	FieldValue getField(AVCLanMsgField field);
	void setField(AVCLanMsgField field, FieldValue value);

	static bool isAckBit(uint8_t bitPos);

	static bool	calculateParity(FieldValue data);

	uint32_t getMessageLength();

	const char* toString();
	static AVCLanMsg fromString(const char* str);
};

#endif /* AVCLAN_AVCLANMSG_H_ */
