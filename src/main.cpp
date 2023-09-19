#include <Driver.h>
#include <Protocol.h>
#include <CDChanger.h>

#include <stdio.h>
#include <stdlib.h>
#include <VCoreCommunication.h>

#include <memory>

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
	power power;
	power::turnOn(power::Usb5V);
	power::turnOn(power::VCore3P3V);
	power::turnOn(power::VCore5V);
	power::turnOn(power::VBase5V);

	power::turnOn(power::AudioAmp);
	power::turnOn(power::VideoSel);
	power::turnOn(power::VideoReset);

	p_timer timer(2);

	Driver avcLan(timer);
	Protocol avcLanProtocol(avcLan);

	CDChanger cdChanger;
	avcLanProtocol.addDevice(cdChanger);

	//VCoreCommunication::uartVCore.receiveComplete = &VCoreCommunication::uartReceiveComplete;
	//VCoreCommunication::startUartReceive();

	uint32_t waitUntil = timer.getTicks() + 1000000;
	while(timer.getTicks() < waitUntil) {}
	avcLan.onTimerCallback();

	while(1) {
		waitUntil = timer.getTicks() + 1000000;
		while(timer.getTicks() < waitUntil) {
			while(!avcLan.receiveQueue.empty()) {
				std::shared_ptr<IEBusMessage> ieBusMessage = avcLan.receiveQueue.front();

				AVCLanMessage avcLanMessage(*ieBusMessage);
				avcLanProtocol.onMessage(avcLanMessage);

				VCoreCommunication::onMessageReceived(ieBusMessage);

				avcLan.receiveQueue.pop();
			}

			avcLan.poll();
		}

		//uartOut.printf("%d\r\n", timer.getTicks());

		if(cdChanger.isInitialized) {
			cdChanger.sendStatusPlayback();
			/*
			if (!cdChanger.haveSentTOC) {
				cdChanger.sendTOC();
				cdChanger.haveSentTOC = true;
				cdChanger.sendCDInserted();
			}
			 */
		} else {
			cdChanger.sendFunctionMappingRequest();
		}
	}
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
