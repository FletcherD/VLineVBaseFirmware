/*
 * Protocol.h
 *
 *  Created on: Aug 11, 2023
 *      Author: fletcher
 */
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "AVCLanMessage.h"
#include "Driver.h"
#include "Device.h"

#include <vector>

bool isMessageForAddress(const AVCLanMessage&, Address);

class Protocol {
private:
	Driver& driver;

	std::vector<std::shared_ptr<Device>> devices;
public:
	Protocol(Driver&);

	void addDevice(Device& device);

	void onMessage(const IEBusMessage&);
	void sendMessage(const AVCLanMessage&);
};

#endif /* PROTOCOL_H_ */
