#include "VCoreCommunication.h"

uart VCoreCommunication::uartVCore(3, 1500000);

void VCoreCommunication::onMessageReceived(AVCLanMsg message) {
	uart::SendData uartSendData;
	uartSendData.size = AVCLanMsg::MaxMessageLenBytes;
	uartSendData.data = new uint8_t[AVCLanMsg::MaxMessageLenBytes];
	for(uint8_t i = 0; i != AVCLanMsg::MaxMessageLenBytes; i++) {
		uartSendData.data[i] = message.messageBuf[i];
	}
	uartVCore.queueSend(uartSendData);
}

void VCoreCommunication::receiveFromUart() {
	uartVCore.USARTdrv->Receive(uartReceiveMsg.messageBuf, AVCLanMsg::MaxMessageLenBytes);
}
void VCoreCommunication::receiveComplete() {
	uartOut.printf("Sending message: ");
	uartOut.printf(uartReceiveMsg.toString());
	AVCLanDrvRxTx::instance->sendMessage(uartReceiveMsg);
	receiveFromUart();
}
