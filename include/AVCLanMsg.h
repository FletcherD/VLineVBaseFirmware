/*
 * AVCLanMsg.h
 *
 *  Created on: Jul 24, 2023
 *      Author: fletcher
 */
#include "cmsis_device.h"
#include "util.h"

#include <vector>

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
	static constexpr AVCLanMsgField Data(uint8_t N) {
		return AVCLanMsgField( { BitOffset: 44+  (10*N), LengthBits: 8,	IsAck: false } );
	}
	static constexpr AVCLanMsgField Data_P(uint8_t N) {
		return AVCLanMsgField( { BitOffset:	44+8+(10*N), LengthBits: 1,	IsAck: false } );
	}
	static constexpr AVCLanMsgField Data_A(uint8_t N) {
		return AVCLanMsgField( { BitOffset:	44+9+(10*N), LengthBits: 1, IsAck: true } );
	}

	static AVCLanMsgField nextField(const AVCLanMsgField field) {
		if(field == Broadcast)			return MasterAddress;
		if(field == MasterAddress)		return MasterAddress_P;
		if(field == MasterAddress_P)	return SlaveAddress;
		if(field == SlaveAddress)		return SlaveAddress_P;
		if(field == SlaveAddress_P)		return SlaveAddress_A;
		if(field == SlaveAddress_A)		return Control;
		if(field == Control)			return Control_P;
		if(field == Control_P)			return Control_A;
		if(field == Control_A)			return DataLength;
		if(field == DataLength)			return DataLength_P;
		if(field == DataLength_P)		return DataLength_A;
		if(field == DataLength_A)		return Data(0);
		for(uint8_t i = 0; i < 32; i++) {
			if(field == Data(i))		return Data_P(i);
			if(field == Data_P(i))		return Data_A(i);
			if(field == Data_A(i))		return Data(i+1);
		}
	}

	static constexpr uint8_t messageBufLen = 32+sizeof(FieldValue);
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

	AVCLanMsgField getFieldAt(uint32_t bitPos);

	static bool	calculateParity(FieldValue data);

	uint32_t getMessageLength();

};

#endif /* AVCLAN_AVCLANMSG_H_ */
