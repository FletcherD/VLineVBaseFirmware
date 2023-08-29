/*
 * AVCLanDevice.h
 *
 *  Created on: Aug 11, 2023
 *      Author: fletcher
 */

#ifndef AVCLANDEVICE_H_
#define AVCLANDEVICE_H_

#include "MessageRaw.h"
#include <map>

class Operations {
public:
	static constexpr Opcode RegisterDevicesRequest 	= 0x00;
	static constexpr Opcode LanInit 				= 0x01;
	static constexpr Opcode DeviceMapping 			= 0x02;
	static constexpr Opcode RegisterDevices 		= 0x10;
	static constexpr Opcode DevicesRequest 			= 0x12;
	static constexpr Opcode Ping 					= 0x20;
	static constexpr Opcode Pong 					= 0x30;
};

class Device {
public:
=======
#include <functional>

class Device {
public:
	// TODO: 0x00 means master, 0x10 means slave. Rename?
	static constexpr Opcode ListDevicesRequest 		= 0x00;
	static constexpr Opcode ListDevicesResponse 	= 0x10;
	static constexpr Opcode RestartLan 				= 0x01;
	static constexpr Opcode DeviceMappingResponse 	= 0x02;
	static constexpr Opcode DeviceMappingRequest 	= 0x12;
	static constexpr Opcode PingRequest 			= 0x20;
	static constexpr Opcode PingResponse 			= 0x30;
	static constexpr Opcode EnableFunctionRequest	= 0x42;
	static constexpr Opcode EnableFunctionResponse	= 0x52;
	static constexpr Opcode DisableFunctionRequest	= 0x43;
	static constexpr Opcode DisableFunctionResponse	= 0x53;

	Address address;
	std::vector<LogicalDevice> devices;

	typedef void (Device::*MessageHandler)(Message);
private:
protected:
	std::map<LogicalDevice, Address> deviceAddressMap;
	std::map<Opcode, MessageHandler> messageHandlerMap;

public:
	Device(Address, std::vector<LogicalDevice> devices);

	void onMessage(Message);
	std::function<void(Message)> sendMessageCallback;
	Message createResponseMessage(Message messageIn);

	// Message handlers -------------------

	void handler_RegisterDevicesRequest(Message);
	void handler_DeviceMapping(Message);
	void handler_Ping(Message);
};

#endif /* AVCLANDEVICE_H_ */
