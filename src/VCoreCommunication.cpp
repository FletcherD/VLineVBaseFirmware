#include "VCoreCommunication.h"

uart VCoreCommunication::uartVCore(0, 921600);

void VCoreCommunication::onMessageReceived(AVCLanMsg message) {
	uart::SendData uartSendData;
	uartSendData.size = AVCLanMsg::MaxMessageLenBytes;
	uartSendData.data = new char[AVCLanMsg::MaxMessageLenBytes];
	for(uint8_t i = 0; i != AVCLanMsg::MaxMessageLenBytes; i++) {
		uartSendData.data[i] = message.messageBuf[i];
	}
	uartVCore.queueSend(uartSendData);
}

void VCoreCommunication::receiveFromUart() {
	uartVCore.USARTdrv->Receive(uartReceiveMsg.messageBuf, AVCLanMsg::MaxMessageLenBytes);
}
void VCoreCommunication::receiveComplete() {
	char messageStr[128];
	uartReceiveMsg.toString(messageStr);
	uartOut.printf("Sending message: %s\r\n", messageStr);
	AVCLanDrvRxTx::instance->sendMessage(uartReceiveMsg);
	receiveFromUart();
}

