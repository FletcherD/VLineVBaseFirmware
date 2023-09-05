/*
 * Protocol.h
 *
 *  Created on: Aug 11, 2023
 *      Author: fletcher
 */
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "MessageRaw.h"
#include "Driver.h"
#include "Device.h"

#include <vector>

Message decodeMessage(const MessageRaw);
MessageRawPtr encodeMessage(const Message);

bool isMessageForAddress(const Message, Address);

class Protocol {
private:
	Driver& driver;

	std::vector<Device> devices;
public:
	Protocol(Driver&);

	void addDevice(Device device);

	void onMessageRaw(const MessageRaw);
	void sendMessage(const Message);
};

#endif /* PROTOCOL_H_ */
