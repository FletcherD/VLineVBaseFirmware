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

class AVCLanMsg {
public:
	struct AVCLanMsgField {
		uint8_t 	BitOffset;
		uint8_t 	LengthBits;
	};

	typedef enum
	{   // No this is not a mistake, broadcast = 0!
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

private:
	uint8_t* messageBuf;

public:
	AVCLanMsg(uint8_t*);

	template<typename T> T
	getField(AVCLanMsgField field)
	{
		uint8_t lenBytes = sizeof(T);
		uint8_t startByte = field.BitOffset / 8;
		T bitMask = (1UL<<field.LengthBits) - 1;
		uint8_t bitShift = (lenBytes*8) - field.LengthBits - (field.BitOffset%8);

		T value = *(T*)(messageBuf+startByte);
		value = swapBytes(value);
		value = (value >> bitShift) & bitMask;
		return value;
	}

	template<typename T> bool
	calculateParity(T data)
	{
		bool parity = false;
		while(data != 0) {
			if(data & 1UL)
				parity = !parity;
			data = (data >> 1);
		}
		return parity;
	}

	uint32_t
	getMessageLength()
	{
		static constexpr uint32_t dataFieldLength =
				AVCLanMsg::Data(0).LengthBits +
				AVCLanMsg::Data_P(0).LengthBits +
				AVCLanMsg::Data_A(0).LengthBits;
		static constexpr uint32_t dataFieldOffset = AVCLanMsg::Data(0).BitOffset;
		uint8_t dataLen = getField<uint32_t>(DataLength);
		return dataFieldOffset + dataLen*dataFieldLength;
	}

};

#endif /* AVCLAN_AVCLANMSG_H_ */
