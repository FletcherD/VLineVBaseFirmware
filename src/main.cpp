#include <stdio.h>
#include <stdlib.h>

#include "diag/trace.h"
#include "timer.h"

#include "vcore_power.h"
#include "AVCLanDrv.h"

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

	AVCLanDrv avcLan;

	uint32_t t = 0;
	while(1) {
		uartOut.printf("Idle %d\r\n", t++);
		avcLan.timer.sleep(1000000);
	}
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
