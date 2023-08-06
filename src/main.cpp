#include <AVCLanDrvRxTx.h>
#include <stdio.h>
#include <stdlib.h>
#include <VCoreCommunication.h>

#include "diag/trace.h"
#include "timer.h"

#include "power.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main (int argc, char* argv[])
{
	uartOut.printf("Hello Arm World!\r\n");
	uartOut.printf("System clock: %u Hz\r\n", SystemCoreClock);

	power power;
	power.turn_on(power::Usb5V);
	power.turn_on(power::VCore3P3V);
	power.turn_on(power::VCore5V);
	power.turn_on(power::VBase5V);

	power.turn_on(power::AudioAmp);

	p_timer timer = p_timer(2);
	AVCLanDrvRxTx avcLan(timer);

	avcLan.messageReceivedCallback = &VCoreCommunication::onMessageReceived;

	uint32_t t = 0;
	while(1) {
		timer.sleep(1000000);
		//VCoreCommunication::uartVCore.printf("Idle %d\r\n", t++);
		//uartOut.printf("Idle %d\r\n", t++);

		//AVCLanMsg messageBeep(AVCLanMsg::DIRECT, 0x110, 0x440, 0xf, std::vector<uint8_t>({0x0, 0x5e, 0x29, 0x60, 0x80}) );
		//AVCLanMsg messagePing(AVCLanMsg::BROADCAST, 0x110, 0xfff, 0xf, std::vector<uint8_t>({0x12, 0x01, 0x20, 0x69}) );
		//AVCLanMsg messageGetDevices(AVCLanMsg::BROADCAST, 0x110, 0xfff, 0xf, std::vector<uint8_t>({0x12, 0x01, 0x00}) );
		//avcLan.sendMessage(messagePing);
	}
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
