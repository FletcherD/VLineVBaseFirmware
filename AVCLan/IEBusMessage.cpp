/*
 * IEBusMessage.cpp
 *
 *  Created on: Sep 8, 2023
 *      Author: fletcher
 */
#include "IEBusMessage.h"

uint32_t IEBusMessage::getMessageLength() const
{
	return IEBusDataField(0).bitOffset + dataLength*DataFieldLength;
}
