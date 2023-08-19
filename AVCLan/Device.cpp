/*
 * Device.cpp
 *
 *  Created on: Aug 11, 2023
 *      Author: fletcher
 */
#include "Device.h"

Device::Device(Address address)
	: address(address)
{
	messageHandlerMap[Operations::RegisterDevicesRequest] = &Device::handler_RegisterDevicesRequest;
}

void
Device::handler_RegisterDevicesRequest(Message message) {

}
