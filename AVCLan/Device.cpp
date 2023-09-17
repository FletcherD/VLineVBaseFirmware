/*
 * Device.cpp
 *
 *  Created on: Aug 11, 2023
 *      Author: fletcher
 */
#include "Device.h"

#include <utility>

#include "diag/trace.h"

Device::Device(Address address, std::vector<Function> functions)
	: address(address),
	  functions(std::move(functions))
{
	messageHandlerMap[ListFunctionsRequest] = &Device::handler_ListFunctionsRequest;
	messageHandlerMap[RestartLan] = &Device::handler_ListFunctionsRequest;
	messageHandlerMap[FunctionMappingResponse] = &Device::handler_FunctionMappingResponse;
	messageHandlerMap[PingRequest] = &Device::handler_Ping;
	messageHandlerMap[EnableFunctionRequest] = &Device::handler_EnableFunctionRequest;
	messageHandlerMap[DisableFunctionRequest] = &Device::handler_DisableFunctionRequest;
}

void
Device::onMessage(AVCLanMessage messageIn)
{
	if(messageHandlerMap.count(messageIn.opcode) != 0) {
		MessageHandler handler = messageHandlerMap[messageIn.opcode];
		std::invoke(handler, this, messageIn);
	}
}

void
Device::handler_ListFunctionsRequest(AVCLanMessage messageIn)
{
	AVCLanMessage listFnResponse = createResponseMessage(messageIn);
	listFnResponse.opcode = ListFunctionsResponse;
	listFnResponse.setOperands(functions);
	std::invoke(sendMessageCallback, listFnResponse);

	AVCLanMessage fnMapRequest = createResponseMessage(messageIn);
	fnMapRequest.dstFunction = 0x12;
	fnMapRequest.opcode = FunctionMappingRequest;
	fnMapRequest.setOperands(functionsRequested);
	std::invoke(sendMessageCallback, fnMapRequest);
}

void
Device::handler_Ping(AVCLanMessage messageIn)
{
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = PingResponse;
	responseMsg.setOperands({messageIn.operands[0], 0xff});
	std::invoke(sendMessageCallback, responseMsg);
}

void
Device::handler_FunctionMappingResponse(AVCLanMessage messageIn)
{
	for(size_t i = 0; i < messageIn.nOperands; i+=2) {
		Function logicalDev = messageIn.operands[i+1] & 0xF;
		Address networkDev = messageIn.operands[i] << 4 | messageIn.operands[i+1] >> 4;
		functionAddressMap[logicalDev] = networkDev;
	}

	isInitialized = true;
}


void
Device::handler_EnableFunctionRequest(AVCLanMessage messageIn)
{
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = EnableFunctionResponse;
	responseMsg.setOperands(messageIn.getOperands());
	std::invoke(sendMessageCallback, responseMsg);
}

void
Device::handler_DisableFunctionRequest(AVCLanMessage messageIn)
{
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = DisableFunctionResponse;
	responseMsg.setOperands(messageIn.getOperands());
	std::invoke(sendMessageCallback, responseMsg);
}

void
Device::sendFunctionMappingRequest()
{
	AVCLanMessage responseMsg(IEBusMessage{
		.broadcast 		= UNICAST,
		.masterAddress 	= address,
		.slaveAddress 	= 0x110,
	});
	responseMsg.srcFunction = 0x01;
	responseMsg.dstFunction = 0x12;
	responseMsg.opcode = FunctionMappingRequest;
	responseMsg.setOperands(functionsRequested);
	std::invoke(sendMessageCallback, responseMsg);
}

AVCLanMessage
Device::createResponseMessage(AVCLanMessage messageIn)
{
	IEBusMessage ieBusMessage = {
		.broadcast		= UNICAST,
		.masterAddress	= address,
		.slaveAddress	= messageIn.masterAddress,
		.control		= 0xf,
	};
	AVCLanMessage avcLanMessage(ieBusMessage);
	avcLanMessage.srcFunction = messageIn.dstFunction;
	avcLanMessage.dstFunction = messageIn.srcFunction;
	return avcLanMessage;
}
