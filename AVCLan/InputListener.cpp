//
// Created by fletcher on 9/17/23.
//

#include "InputListener.h"

InputListener::InputListener()
	: Device(0x1D3, {0xb0, 0x43, 0x24, 0x31, 0x25})
{
	// Don't want this to act like a regular device; just listen to messages passively
	messageHandlerMap.clear();

	addHandler(TouchEvent, (MessageHandler)&InputListener::handle_TouchEvent);
}

void InputListener::handle_TouchEvent(AVCLanMessage messageIn) {
	uint8_t xCoord = messageIn.operands[0];
	uint8_t yCoord = messageIn.operands[1];
	// TODO do something with it...
}
