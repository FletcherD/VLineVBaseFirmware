#include <Driver.h>
#include <Protocol.h>
#include <CDChanger.h>

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

	Driver avcLan(timer);
	Protocol avcLanProtocol(avcLan);

	CDChanger exampleDevice;
	avcLanProtocol.addDevice(exampleDevice);

	//VCoreCommunication::uartVCore.receiveComplete = &VCoreCommunication::uartReceiveComplete;
	//VCoreCommunication::startUartReceive();

	//MessageRaw messageRaw(UNICAST, 0x123, 0x1d3, 0xf, {0x00, 0x12, 0x01, 0x00});
	//avcLanProtocol.onMessageRaw(messageRaw);

	while(1) {
		uint32_t waitUntil = timer.getTicks() + 1000000;
		while(timer.getTicks() < waitUntil) {
			while(!avcLan.receiveQueue.empty()) {
				MessageRaw& messageRaw = *avcLan.receiveQueue.front();

				VCoreCommunication::onMessageReceived(messageRaw);
				avcLanProtocol.onMessageRaw(messageRaw);

				avcLan.receiveQueue.pop();
			}

			avcLan.poll();
		}
		trace_printf("Bit Errors: %d - Total Msgs: %d - Mode: %d", avcLan.bitErrorCount, avcLan.totalMsgCount, avcLan.operatingMode);
		//trace_printf("RXCount: %d", VCoreCommunication::uartVCore.USARTdrv->GetRxCount());
		//VCoreCommunication::uartVCore.printf("Idle %d\r\n", t++);
		//uartOut.printf("Idle %d\r\n", t++);
		//trace_printf("TX Send Bit: %d", avcLan.sendBitPos);

		//MessageRaw messageBeep(UNICAST, 0x110, 0x440, 0xf, {0x0, 0x5e, 0x29, 0x60, 0x80} );
		//AVCLanMsg messagePing(AVCLanMsg::BROADCAST, 0x110, 0xfff, 0xf, std::vector<uint8_t>({0x12, 0x01, 0x20, 0x69}) );
		//AVCLanMsg messageGetDevices(AVCLanMsg::BROADCAST, 0x110, 0xfff, 0xf, std::vector<uint8_t>({0x12, 0x01, 0x00}) );
		//avcLan.sendMessage(messageBeep);
	}
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
