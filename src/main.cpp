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
	power.turn_on(power::Usb5V);
	power.turn_on(power::VCore3P3V);
	power.turn_on(power::VCore5V);
	power.turn_on(power::VBase5V);

	//power.turn_on(power::AudioAmp);

	p_timer timer(2);

	Driver avcLan(timer);
	Protocol avcLanProtocol(avcLan);

	CDChanger exampleDevice;
	avcLanProtocol.addDevice(exampleDevice);

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
				//VCoreCommunication::onMessageReceived(messageRaw);
				avcLanProtocol.onMessage(avcLanMessage);

                /*
				trace_printf("%d %03x %03x %01x %d : %d",
						ieBusMessage->broadcast,
						ieBusMessage->masterAddress,
						ieBusMessage->slaveAddress,
						ieBusMessage->control,
						ieBusMessage->dataLength,
						ieBusMessage->data[0]);
                 */

				avcLan.receiveQueue.pop();
			}

			avcLan.poll();
		}

		/*
        IEBusMessage ieBusMessage = {
                .broadcast      =  UNICAST,
                .masterAddress  =  0x1d3,
                .slaveAddress   =  0x190,
                .control        =  0xf
        };
        AVCLanMessage avcLanMessage(ieBusMessage);
		avcLanMessage.srcFunction = 0x01;
		avcLanMessage.dstFunction = 0x02;
		avcLanMessage.opcode = 0x03;
        avcLanMessage.setOperands({4,5,6,7,8,9});
        avcLan.sendMessage(std::make_shared<IEBusMessage>(avcLanMessage));
        */
		//trace_printf("Bit Errors: %d - Total Msgs: %d - Mode: %d", avcLan.bitErrorCount, avcLan.totalMsgCount, avcLan.operatingMode);
	}
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
