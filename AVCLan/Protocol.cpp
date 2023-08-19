/*
 * Protocol.cpp
 *
 *  Created on: Aug 11, 2023
 *      Author: fletcher
 */
#include "Protocol.h"

Message decodeMessage(const MessageRaw message)
{
	Message r;
	r.broadcast 		= (BroadcastValue)message.getField(MessageRaw::Broadcast);
	r.masterAddress 	= message.getField(MessageRaw::MasterAddress);
	r.slaveAddress 		= message.getField(MessageRaw::SlaveAddress);
	r.control 			= message.getField(MessageRaw::Control);
	uint8_t dataLen 	= message.getField(MessageRaw::DataLength);
	uint8_t operationStartPos = (r.broadcast==BROADCAST ? 0 : 1);
	r.opcode 			= message.getField(MessageRaw::Data(operationStartPos++));
	r.operands 			= std::vector<DataValue>();
	while(operationStartPos != dataLen) {
		r.operands.push_back( message.getField(MessageRaw::Data(operationStartPos++)) );
	}
	return r;
}

bool isMessageForAddress(const Message message, Address address) {
	if(message.broadcast == BROADCAST) {
		return message.slaveAddress == address;
	} else {

	}
}

