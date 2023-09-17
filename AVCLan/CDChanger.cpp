/*
 * CDChanger.cpp
 *
 *  Created on: Aug 18, 2023
 *      Author: fletcher
 */

#include "CDChanger.h"

CDChanger::CDChanger()
	: Device(0x1D3, {0xb0, 0x43, 0x24, 0x31, 0x25})
{
	functionsRequested = {0xb0, 0x29, 0x43, 0x24, 0x31, 0x25, 0x42, 0x8c};
	messageHandlerMap[RequestStatus] 	= (MessageHandler)&CDChanger::handler_RequestStatus;
	messageHandlerMap[RequestPlayback] 	= (MessageHandler)&CDChanger::handler_RequestPlayback;
	messageHandlerMap[RequestPlayback2] = (MessageHandler)&CDChanger::handler_RequestPlayback2;
	messageHandlerMap[RequestLoader] 	= (MessageHandler)&CDChanger::handler_RequestLoader;
	messageHandlerMap[RequestLoader2] 	= (MessageHandler)&CDChanger::handler_RequestLoader2;
	messageHandlerMap[RequestToc] 		= (MessageHandler)&CDChanger::handler_RequestToc;
	messageHandlerMap[RequestStatusFA] 	= (MessageHandler)&CDChanger::handler_RequestStatusFA;
	messageHandlerMap[RequestTrackName] = (MessageHandler)&CDChanger::handler_RequestTrackName;

	messageHandlerMap[Function07Request] = (MessageHandler)&CDChanger::handler_Function07Request;
}


void CDChanger::handler_RequestStatus(AVCLanMessage messageIn) {
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportStatus;
    responseMsg.setOperands({0x06, 0x13, 0x90, 0xf1, 0x01, 0x03});
	std::invoke(sendMessageCallback, responseMsg);
}

void CDChanger::handler_RequestPlayback(AVCLanMessage messageIn) {
	sendPlayback();
}
// TODO: send this every second
void CDChanger::sendPlayback() {
	AVCLanMessage responseMsg(IEBusMessage{
		.broadcast 		= BROADCAST,
		.masterAddress 	= address,
		.slaveAddress 	= 0x1ff,
	});
	responseMsg.srcFunction = 0x43;
	responseMsg.dstFunction = 0x31;
	responseMsg.opcode = ReportPlayback;
	responseMsg.setOperands({0x01, 0x10, 0x01, 0x69, 0x00, playbackSeconds, 0x00, 0x00, 0x94, 0x00});
	playbackSeconds++;
	std::invoke(sendMessageCallback, responseMsg);
}

void CDChanger::handler_RequestPlayback2(AVCLanMessage messageIn) {
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportPlayback2;
    responseMsg.setOperands({0x00, 0x30, 0x01, 0x01, 0x00, 0x06, 0x00, 0x80});
	std::invoke(sendMessageCallback, responseMsg);
}

void CDChanger::handler_RequestLoader(AVCLanMessage messageIn) {
}

void CDChanger::handler_RequestLoader2(AVCLanMessage messageIn) {
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportLoader2;
    responseMsg.setOperands({0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x02});
	std::invoke(sendMessageCallback, responseMsg);
}

void CDChanger::handler_RequestToc(AVCLanMessage messageIn) {
	sendTOC();
}
void CDChanger::sendTOC() {
	AVCLanMessage responseMsg(IEBusMessage{
		.broadcast 		= BROADCAST,
		.masterAddress 	= address,
		.slaveAddress 	= 0x1ff,
	});
	responseMsg.srcFunction = 0x43;
	responseMsg.dstFunction = 0x25;
	responseMsg.opcode = ReportToc;
	responseMsg.setOperands({0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01});
	std::invoke(sendMessageCallback, responseMsg);
}

void CDChanger::handler_RequestTrackName(AVCLanMessage messageIn) {
}

void CDChanger::handler_RequestStatusFA(AVCLanMessage messageIn) {
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = ReportStatusFA;
	responseMsg.setOperands({0x01, 0x00, 0x69, 0x77, 0x59, 0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01});
	std::invoke(sendMessageCallback, responseMsg);
}

void CDChanger::handler_Function07Request(AVCLanMessage messageIn)
{
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = Function07Response;
	responseMsg.setOperands({});
	std::invoke(sendMessageCallback, responseMsg);

	AVCLanMessage responseMsgB0 = createResponseMessage(messageIn);
	responseMsgB0.opcode = StatusB0;
	responseMsgB0.dstFunction = 0x12;
	responseMsgB0.setOperands({});
	std::invoke(sendMessageCallback, responseMsgB0);

	AVCLanMessage responseMsgCd1(IEBusMessage{
		.broadcast 		= UNICAST,
		.masterAddress 	= address,
		.slaveAddress 	= 0x110,
	});
	responseMsgCd1.srcFunction = 0xb0;
	responseMsgCd1.dstFunction = 0x12;
	responseMsgCd1.opcode = CdInserted;
	responseMsgCd1.setOperands({0x0a});
	std::invoke(sendMessageCallback, responseMsgCd1);

	AVCLanMessage responseMsgCd2(IEBusMessage{
		.broadcast 		= UNICAST,
		.masterAddress 	= address,
		.slaveAddress 	= 0x110,
	});
	responseMsgCd2.srcFunction = 0x43;
	responseMsgCd2.dstFunction = 0x12;
	responseMsgCd2.opcode = CdInserted;
	responseMsgCd2.setOperands({0x01});
	std::invoke(sendMessageCallback, responseMsgCd2);
}

void CDChanger::sendCDInserted() {
}