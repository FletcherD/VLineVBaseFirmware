#include "VCoreCommunication.h"

#include "diag/trace.h"

uart VCoreCommunication::uartVCore(0, 921600);

AVCLanMsg VCoreCommunication::uartReceiveMsg;

void VCoreCommunication::onMessageReceived(AVCLanMsg message) {
	/*
	uart::SendData uartSendData;
	uartSendData.size = AVCLanMsg::MaxMessageLenBytes;
	uartSendData.data = new char[AVCLanMsg::MaxMessageLenBytes];
	for(uint8_t i = 0; i != AVCLanMsg::MaxMessageLenBytes; i++) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		uartSendData.data[i] = message.messageBuf[AVCLanMsg::MaxMessageLenBytes-i-1];
#else
		uartSendData.data[i] = message.messageBuf[i];
#endif
	}
	*/
	char messageStr[128];
	message.toString(messageStr);
	uartVCore.printf("%s\r\n", messageStr);
	//uartVCore.queueSend(uartSendData);
}

void VCoreCommunication::startUartReceive() {
	int32_t errNo = uartVCore.USARTdrv->Receive(uartReceiveMsg.messageBuf, AVCLanMsg::MaxMessageLenBytes);
}

void VCoreCommunication::uartReceiveComplete() {
	char messageStr[128];
	uartReceiveMsg.toString(messageStr);
	if(uartReceiveMsg.isValid()) {
		trace_printf("Sending message: %s\n", messageStr);
		AVCLanDrvRxTx::instance->sendMessage(uartReceiveMsg);
	} else {
		trace_printf("Error! Want to send invalid message: %s\n", messageStr);
	}
	startUartReceive();
}

