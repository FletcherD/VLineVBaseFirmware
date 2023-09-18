#include "VCoreCommunication.h"

#include "diag/trace.h"

//uart VCoreCommunication::uartVCore(0, 1500000);

uint8_t* VCoreCommunication::uartReceiveByte = uartReceiveBuf;
uint8_t VCoreCommunication::uartReceiveBuf[uartReceiveBufSize];

void VCoreCommunication::onMessageReceived(const std::shared_ptr<IEBusMessage>& message) {
	uartOut.printf("%c %03x %03x ",
					 (message->broadcast==BROADCAST ? 'B' : '-'),
					 message->masterAddress,
					 message->slaveAddress);
	for(size_t i = 0; i != message->dataLength; i++) {
		uartOut.printf("%02x ", message->data[i]);
	}
	uartOut.printf("\r\n");
}

/*
void VCoreCommunication::startUartReceive() {
	int32_t errNo = uartVCore.USARTdrv->Receive(uartReceiveByte, 1);
}

void VCoreCommunication::uartReceiveComplete() {
	static uint8_t* messageStartPos;

	static constexpr size_t headerLen = 2;
	static constexpr uint8_t header = 0xff;

	uartReceiveByte++;
	size_t uartReceivePos = (uartReceiveByte - uartReceiveBuf);
	if(uartReceivePos >= 2 &&
			(*(uartReceiveByte-1) == header && *(uartReceiveByte-2) == header)) {
		messageStartPos = uartReceiveByte;
	}
	if(uartReceiveByte - messageStartPos == MessageRaw::MaxMessageLenBytes) {
		MessageRawPtr thisMessage(new MessageRaw(messageStartPos));

		if(thisMessage->isValid()) {
			//trace_printf("Sending message: %s\n", messageStr);
			Driver::instance->sendMessage(thisMessage);
		} else {
			char messageStr[256];
			thisMessage->toString(messageStr);
			trace_printf("Error! Want to send invalid message: %s\n", messageStr);
		}

		uartReceiveByte = uartReceiveBuf;
	}

	if(uartReceivePos == uartReceiveBufSize)
		uartReceiveByte = uartReceiveBuf;

	startUartReceive();
}
*/