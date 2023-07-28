/*
 * AVCLanMsg.h
 *
 *  Created on: Jul 24, 2023
 *      Author: fletcher
 */
#include "cmsis_device.h"
#include "util.h"

#ifndef AVCLAN_AVCLANMSG_H_
#define AVCLAN_AVCLANMSG_H_

typedef uint32_t FieldValue;

class AVCLanMsg {
public:
	struct AVCLanMsgField {
		uint8_t 	BitOffset;
		uint8_t 	LengthBits;
		uint8_t		StartByte() {
			return BitOffset/8;
		}
	};

	typedef enum
	{
	    AVC_MSG_DIRECT    = 1,
	    AVC_MSG_BROADCAST = 0
	} AVCLanTransmissionMode;

	static constexpr AVCLanMsgField Broadcast 			= {BitOffset: 0,	LengthBits: 1	};
	static constexpr AVCLanMsgField MasterAddress 		= {BitOffset: 1,	LengthBits: 12	};
	static constexpr AVCLanMsgField MasterAddress_P 	= {BitOffset: 13,	LengthBits: 1	};
	static constexpr AVCLanMsgField SlaveAddress	 	= {BitOffset: 14,	LengthBits: 12	};
	static constexpr AVCLanMsgField SlaveAddress_P 		= {BitOffset: 26,	LengthBits: 1	};
	static constexpr AVCLanMsgField SlaveAddress_A 		= {BitOffset: 27,	LengthBits: 1	};
	static constexpr AVCLanMsgField Control 			= {BitOffset: 28,	LengthBits: 4	};
	static constexpr AVCLanMsgField Control_P 			= {BitOffset: 32,	LengthBits: 1	};
	static constexpr AVCLanMsgField Control_A 			= {BitOffset: 33, 	LengthBits: 1	};
	static constexpr AVCLanMsgField DataLength 			= {BitOffset: 34,	LengthBits: 8	};
	static constexpr AVCLanMsgField DataLength_P 		= {BitOffset: 42,	LengthBits: 1	};
	static constexpr AVCLanMsgField Data(uint8_t N) {
		return AVCLanMsgField( { BitOffset: 44+  (10*N), LengthBits:8 } );
	}
	static constexpr AVCLanMsgField Data_P(uint8_t N) {
		return AVCLanMsgField( { BitOffset:	44+8+(10*N), LengthBits:1 } );
	}
	static constexpr AVCLanMsgField Data_A(uint8_t N) {
		return AVCLanMsgField( { BitOffset:	44+9+(10*N), LengthBits:1 } );
	}

	static constexpr uint8_t messageBufLen = 32+sizeof(FieldValue);
	uint8_t messageBuf[messageBufLen];
	uint32_t lengthBits;

	AVCLanMsg();

	bool getBit(uint32_t bitPos);
	void setBit(uint32_t bitPos, bool value);

	FieldValue getField(AVCLanMsgField field);
	void setField(AVCLanMsgField field, FieldValue value);

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
