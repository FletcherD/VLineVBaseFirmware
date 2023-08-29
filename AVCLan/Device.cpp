/*
 * Device.cpp
 *
 *  Created on: Aug 11, 2023
 *      Author: fletcher
 */
#include "Device.h"

Device::Device(Address address, std::vector<LogicalDevice> devices)
	: address(address),
	  devices(devices)
{
	messageHandlerMap[ListDevicesRequest] = &Device::handler_RegisterDevicesRequest;
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
Device::handler_RegisterDevicesRequest(Message messageIn)
{
	Message responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = ListDevicesResponse;
	responseMsg.operands = devices;
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
Device::handler_DeviceMapping(Message messageIn)
{
	for(size_t i = 0; i < messageIn.operands.size(); i+=2) {
		LogicalDevice logicalDev = messageIn.operands[i+1] & 0xF;
		Address networkDev = messageIn.operands[i] << 4 | messageIn.operands[i+1] >> 4;
		deviceAddressMap[logicalDev] = networkDev;
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
	msg.srcDevice = messageIn.dstDevice;
	msg.dstDevice = messageIn.srcDevice;
	return msg;
}
