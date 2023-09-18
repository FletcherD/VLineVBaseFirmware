#ifndef VLINEVBASEFIRMWARE_INPUTLISTENER_H
#define VLINEVBASEFIRMWARE_INPUTLISTENER_H
#include "Device.h"

class InputListener : public Device {

	static constexpr Opcode TouchEvent 		= 0x78;
	static constexpr Opcode NextTrack 		= 0x88;
	static constexpr Opcode PreviousTrack 	= 0x89;

	InputListener();

	void handle_TouchEvent(AVCLanMessage);

};

#endif //VLINEVBASEFIRMWARE_INPUTLISTENER_H
