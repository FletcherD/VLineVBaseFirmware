/*
 * Device.cpp
 *
 *  Created on: Aug 11, 2023
 *      Author: fletcher
 */
#include "Device.h"

#include "diag/trace.h"

Device::Device(Address address, std::vector<Function> functions)
	: address(address),
	  functions(functions)
{
	messageHandlerMap[ListFunctionsRequest] = &Device::handler_ListFunctionsRequest;
	messageHandlerMap[PingRequest] = &Device::handler_Ping;
}

void
Device::onMessage(Message messageIn)
{
	if(messageHandlerMap.count(messageIn.opcode) != 0) {
		MessageHandler handler = messageHandlerMap[messageIn.opcode];
		std::invoke(handler, this, messageIn);
	}
}

void
Device::handler_ListFunctionsRequest(Message messageIn)
{
	Message responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = ListFunctionsResponse;
	responseMsg.operands = functions;
	std::invoke(sendMessageCallback, responseMsg);

	responseMsg = createResponseMessage(messageIn);
	responseMsg.dstFunction = 0x12;
	responseMsg.opcode = FunctionMappingRequest;
	responseMsg.operands = functionsRequested;
	std::invoke(sendMessageCallback, responseMsg);
}

void
Device::handler_Ping(Message messageIn)
{
	Message responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = PingResponse;
	responseMsg.operands = {messageIn.operands[0], 0xff};
	std::invoke(sendMessageCallback, responseMsg);
}

void
Device::handler_FunctionMappingResponse(Message messageIn)
{
	for(size_t i = 0; i < messageIn.operands.size(); i+=2) {
		Function logicalDev = messageIn.operands[i+1] & 0xF;
		Address networkDev = messageIn.operands[i] << 4 | messageIn.operands[i+1] >> 4;
		functionAddressMap[logicalDev] = networkDev;
	}
}

Message
Device::createResponseMessage(Message messageIn)
{
	Message msg;
	msg.broadcast = UNICAST;
	msg.masterAddress = address;
	msg.slaveAddress = messageIn.masterAddress;
	msg.control = 0xF;
	msg.srcFunction = messageIn.dstFunction;
	msg.dstFunction = messageIn.srcFunction;
	return msg;
}
