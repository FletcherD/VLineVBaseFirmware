#include <stdio.h>
#include <stdlib.h>

#include "diag/trace.h"
#include "timer.h"

#include "vcore_power.h"
#include "AVCLanDrvRxTx.h"
#include "AVCLanMsg.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main (int argc, char* argv[])
{
	uartOut.start();

	uartOut.printf("Hello Arm World!\r\n");
	uartOut.printf("System clock: %u Hz\r\n", SystemCoreClock);

	power power;
	power.turn_on(power::PowerNode::Usb5v);
	power.turn_on(power::PowerNode::Vcore_3p3v);
	power.turn_on(power::PowerNode::Vcore_5v);
	power.turn_on(power::PowerNode::Vbase_5v);

	p_timer timer = p_timer(2);
	AVCLanDrvRxTx avcLan(timer);

	uint32_t t = 0;
	while(1) {
		timer.sleep(2000000);
		uartOut.printf("Idle %d\r\n", t++);

		//AVCLanMsg messageBeep(false, 0x110, 0x440, 0xf, std::vector<uint8_t>({0x0, 0x5e, 0x29, 0x60, 0x80}) );
		AVCLanMsg messagePing(true, 0x110, 0xfff, 0xf, std::vector<uint8_t>({0x12, 0x01, 0x20, 0x69}) );
		avcLan.sendMessage(messagePing);
	}
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
