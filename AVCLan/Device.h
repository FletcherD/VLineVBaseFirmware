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
	Address address;
	std::vector<LogicalDevice> devices;

	typedef void (Device::*MessageHandler)(Message);
private:
	std::map<LogicalDevice, Address> deviceAddressMap;
	std::map<Opcode, MessageHandler> messageHandlerMap;

public:
	Device(Address);

	void onMessage(Message);

	void sendMessage(Message);

	// Message handlers

	void handler_RegisterDevicesRequest(Message);
};

#endif /* AVCLANDEVICE_H_ */
