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

void Protocol::onMessage(IEBusMessage ieBusMessage)
{
	AVCLanMessage avcLanMessage(ieBusMessage);
	for(auto deviceIt = devices.begin(); deviceIt != devices.end(); deviceIt++) {
		if(isMessageForAddress(avcLanMessage, deviceIt->address)) {
			deviceIt->onMessage(avcLanMessage);
		}
	}
}

void Protocol::sendMessage(AVCLanMessage avcLanMessage)
{
	std::shared_ptr<IEBusMessage> ieBusMessage(new IEBusMessage(avcLanMessage));
	driver.sendMessage(ieBusMessage);
}

void Protocol::addDevice(Device device)
{
	device.sendMessageCallback = std::bind(&Protocol::sendMessage, this, std::placeholders::_1);
	devices.push_back(device);
}

bool isMessageForAddress(const AVCLanMessage message, Address address) {
	if(message.broadcast == UNICAST) {
		return message.slaveAddress == address;
	} else {
		// Use address as mask
		return (address & ~message.slaveAddress) == 0;
	}
}

