#include "VCoreCommunication.h"

#include "diag/trace.h"

uart VCoreCommunication::uartVCore(0, 1500000);

uint8_t* VCoreCommunication::uartReceiveByte = uartReceiveBuf;
uint8_t VCoreCommunication::uartReceiveBuf[uartReceiveBufSize];

/*
void VCoreCommunication::onMessageReceived(MessageRaw message) {
	static uint32_t totalMsgs = 0;

	static constexpr size_t headerLen = 2;
	static constexpr uint8_t header = 0xff;
	static constexpr size_t messageLen = headerLen + MessageRaw::MaxMessageLenBytes;

	uart::SendData uartSendData;
	uartSendData.size = messageLen;
	uartSendData.data = new char[messageLen];
	for(size_t i = 0; i < headerLen; i++) {
		uartSendData.data[i] = header;
	}
	for(uint8_t i = 0; i != MessageRaw::MaxMessageLenBytes; i++) {
		size_t j = i+headerLen;
		uartSendData.data[j] = message.messageBuf[i];
	}

	uartVCore.queueSend(uartSendData);

	//trace_printf("Total msgs: %d\n", totalMsgs++);
}

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
