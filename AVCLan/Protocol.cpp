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
	/*
	if(ieBusMessage.masterAddress == 0x110) {
		return;
	}
	 */
	AVCLanMessage avcLanMessage(ieBusMessage);
	for(auto & device : devices) {
		if(isMessageForAddress(avcLanMessage, device.address)) {
			device.onMessage(avcLanMessage);
		}
	}
}

void Protocol::sendMessage(AVCLanMessage avcLanMessage)
{
	std::shared_ptr<IEBusMessage> ieBusMessage(new IEBusMessage(avcLanMessage));
	driver.queueMessage(ieBusMessage);
}

void Protocol::addDevice(Device device)
{
	device.sendMessageCallback = [this](auto && PH1) { sendMessage(std::forward<decltype(PH1)>(PH1)); };
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

