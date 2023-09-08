/*
 * Protocol.cpp
 *
 *  Created on: Aug 11, 2023
 *      Author: fletcher
 */
#include "Protocol.h"

#include "diag/trace.h"

Protocol::Protocol(Driver& driver)
	: driver(driver)
{
}

void Protocol::onMessageRaw(MessageRaw messageRaw)
{
	Message message = decodeMessage(messageRaw);
	for(auto deviceIt = devices.begin(); deviceIt != devices.end(); deviceIt++) {
		if(isMessageForAddress(message, deviceIt->address)) {
			deviceIt->onMessage(message);
		}
	}
}

void Protocol::sendMessage(Message message)
{
	MessageRawPtr messageRaw = encodeMessage(message);
	driver.sendMessage(messageRaw);
}

void Protocol::addDevice(Device device)
{
	device.sendMessageCallback = std::bind(&Protocol::sendMessage, this, std::placeholders::_1);
	devices.push_back(device);
}

Message decodeMessage(const MessageRaw message)
{
	Message r;
	r.broadcast 		= (BroadcastValue)message.getField(MessageRaw::Broadcast);
	r.masterAddress 	= message.getField(MessageRaw::MasterAddress);
	r.slaveAddress 		= message.getField(MessageRaw::SlaveAddress);
	r.control 			= message.getField(MessageRaw::Control);
	uint8_t dataLen 	= message.getField(MessageRaw::DataLength);
	uint8_t dataPos = (r.broadcast==BROADCAST ? 0 : 1);
	r.srcFunction		= message.getField(MessageRaw::Data(dataPos++));
	r.dstFunction		= message.getField(MessageRaw::Data(dataPos++));
	r.opcode 			= message.getField(MessageRaw::Data(dataPos++));
	r.operands 			= std::vector<DataValue>();
	while(dataPos != dataLen) {
		r.operands.push_back( message.getField(MessageRaw::Data(dataPos++)) );
	}
	return r;
}

MessageRawPtr encodeMessage(const Message message)
{
	std::vector<DataValue> data;
	if(message.broadcast==UNICAST) {
		data.push_back(0x00);
	}
	data.push_back(message.srcFunction);
	data.push_back(message.dstFunction);
	data.push_back(message.opcode);
	for(auto opIt = message.operands.cbegin(); opIt != message.operands.cend(); opIt++) {
		data.push_back(*opIt);
	}
	MessageRawPtr rPtr(new MessageRaw(
			message.broadcast,
			message.masterAddress,
			message.slaveAddress,
			message.control,
			data) );
	return rPtr;
}

bool isMessageForAddress(const Message message, Address address) {
	if(message.broadcast == UNICAST) {
		return message.slaveAddress == address;
	} else {
		// Use address as mask
		return (address & ~message.slaveAddress) == 0;
	}
}

