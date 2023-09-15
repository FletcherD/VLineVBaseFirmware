/*
 * AVCLanDevice.h
 *
 *  Created on: Aug 11, 2023
 *      Author: fletcher
 */

#ifndef AVCLANDEVICE_H_
#define AVCLANDEVICE_H_

#include "AVCLanMessage.h"
#include <map>
#include <functional>

class Device {
	/*
	 * Implements a AVC-LAN device.
	 * When a message is received, it will be sent to this device
	 * if the slave address of the message matches the device address.
	 * The device can have handlers for various message opcodes.
	 * This is the base class; it implements several AVC-LAN functions
	 * that all devices are required to implement,
	 * such as Ping and ListFunctionsRequest.
	 * Devices derived from this will implement those messages automatically.
	 */
public:
	// TODO: 0x00 means master, 0x10 means slave. Rename?
	static constexpr Opcode ListFunctionsRequest 	= 0x00;
	static constexpr Opcode ListFunctionsResponse 	= 0x10;
	static constexpr Opcode RestartLan 				= 0x01;
	static constexpr Opcode FunctionMappingResponse	= 0x02;
	static constexpr Opcode FunctionMappingRequest 	= 0x12;
	static constexpr Opcode Function07Request		= 0x07; // TODO what is this?
	static constexpr Opcode Function07Response	 	= 0x17; // TODO what is this?
	static constexpr Opcode PingRequest 			= 0x20;
	static constexpr Opcode PingResponse 			= 0x30;
	static constexpr Opcode EnableFunctionRequest	= 0x42;
	static constexpr Opcode EnableFunctionResponse	= 0x52;
	static constexpr Opcode DisableFunctionRequest	= 0x43;
	static constexpr Opcode DisableFunctionResponse	= 0x53;

	Address address;
	std::vector<Function> functions;
	std::vector<Function> functionsRequested;

	typedef void (Device::*MessageHandler)(AVCLanMessage);

protected:

	std::map<Function, Address> functionAddressMap;
	std::map<Opcode, MessageHandler> messageHandlerMap;

public:
	Device(Address, std::vector<Function> functions);

	void onMessage(AVCLanMessage);
	std::function<void(AVCLanMessage)> sendMessageCallback;
	AVCLanMessage createResponseMessage(AVCLanMessage messageIn);

	// Message handlers -------------------

	void handler_ListFunctionsRequest(AVCLanMessage);
	void handler_FunctionMappingResponse(AVCLanMessage);
	void handler_Ping(AVCLanMessage);

	void handler_Function07Request(AVCLanMessage);

	void handler_EnableFunctionRequest(AVCLanMessage);
	void handler_DisableFunctionRequest(AVCLanMessage);

	bool isInitialized = false;
};

#endif /* AVCLANDEVICE_H_ */
