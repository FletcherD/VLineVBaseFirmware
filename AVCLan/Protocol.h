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
MessageRaw encodeMessage(const Message);

bool isMessageForAddress(const Message, Address);

class Protocol {
private:
<<<<<<< HEAD
	Driver driver;

	std::vector<Device> devices;
public:
	Protocol(Driver);
=======
	Driver& driver;

	std::vector<Device> devices;
public:
	Protocol(Driver&);

	void addDevice(Device device);
>>>>>>> 93eff14 (Interim state to allow reading of the code)

	void onMessageRaw(const MessageRaw);
	void sendMessage(const Message);
};

#endif /* PROTOCOL_H_ */
