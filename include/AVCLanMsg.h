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
	};

	typedef enum
	{
	    AVC_MSG_DIRECT    = 1,
	    AVC_MSG_BROADCAST = 0
	} AVCLanTransmissionMode;

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
	static AVCLanMsgField Data(uint8_t N) {
		return AVCLanMsgField( { BitOffset: 44+  (10*N), LengthBits: 8,	IsAck: false } );
	}
	static AVCLanMsgField Data_P(uint8_t N) {
		return AVCLanMsgField( { BitOffset:	44+8+(10*N), LengthBits: 1,	IsAck: false } );
	}
	static AVCLanMsgField Data_A(uint8_t N) {
		return AVCLanMsgField( { BitOffset:	44+9+(10*N), LengthBits: 1, IsAck: true } );
	}

	static AVCLanMsgField nextField(AVCLanMsgField field) {
		switch (field) {
			case Broadcast:			return MasterAddress;
			case MasterAddress:		return MasterAddress_P;
			case MasterAddress_P:	return SlaveAddress;
			case SlaveAddress:		return SlaveAddress_P;
			case SlaveAddress_P:	return SlaveAddress_A;
			case SlaveAddress_A:	return Control;
			case Control:			return Control_A;
			case Control_A:			return Control_P;
			case Control_P:			return DataLength;
			case DataLength:		return DataLength_P;
			case DataLength_P:		return Data(0);
		}
		for(size_t i = 0; i < 32; i++) {
			switch(field) {
				case Data(i):		return Data_P(i);
				case Data_P(i):		return Data_A(i);
				case Data_A(i):		return Data(i+1);
			}
		}
	}

	AVCLanMsgField getCurrentField() {
		AVCLanMsgField field = Broadcast;
		while(true) {
			if(currentBit >= field.BitOffset
			&& currentBit < (field.BitOffset + field.LengthBits)) {
				return field;
			}
			field = nextField(field);
		}
	}

	static constexpr uint8_t messageBufLen = 32+sizeof(FieldValue);
	uint8_t messageBuf[messageBufLen];

	AVCLanMsgField currentField;
	uint32_t currentBit;

	AVCLanMsg();
	AVCLanMsg(bool broadcast,
			uint16_t masterAddress,
			uint16_t slaveAddress,
			uint8_t control,
			std::vector<uint8_t> data);

	bool getBit(uint32_t bitPos);
	void setBit(uint32_t bitPos, bool value);

	bool getNextBit();
	void setNextBit(bool value);

	static bool	calculateParity(FieldValue data)
	{
		bool parity = false;
		while(data != 0) {
			if(data & 1UL)
				parity = !parity;
			data = (data >> 1);
		}
		return parity;
	}

	uint32_t getMessageLength();

};

#endif /* AVCLAN_AVCLANMSG_H_ */
