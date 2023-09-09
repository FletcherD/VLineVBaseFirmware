/*
 * CDChanger.cpp
 *
 *  Created on: Aug 18, 2023
 *      Author: fletcher
 */

#include "CDChanger.h"

CDChanger::CDChanger()
	: Device(0x1D3, {0xB0, 0x43, 0x24, 0x31, 0x25})
{
	functionsRequested = {0xB0, 0x29, 0x43, 0x24, 0x31, 0x25, 0x42, 0x8c};
	messageHandlerMap[RequestStatus] 	= (MessageHandler)&CDChanger::handler_RequestStatus;
	messageHandlerMap[RequestPlayback] 	= (MessageHandler)&CDChanger::handler_RequestPlayback;
	messageHandlerMap[RequestPlayback2] = (MessageHandler)&CDChanger::handler_RequestPlayback2;
	messageHandlerMap[RequestLoader] 	= (MessageHandler)&CDChanger::handler_RequestLoader;
	messageHandlerMap[RequestLoader2] 	= (MessageHandler)&CDChanger::handler_RequestLoader2;
	messageHandlerMap[RequestToc] 		= (MessageHandler)&CDChanger::handler_RequestToc;
	messageHandlerMap[RequestTrackName] = (MessageHandler)&CDChanger::handler_RequestTrackName;
}


void CDChanger::handler_RequestStatus(AVCLanMessage messageIn) {
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportStatus;
    responseMsg.setOperands({0x06, 0x13, 0x90, 0xf1, 0x01, 0x03});
	std::invoke(sendMessageCallback, responseMsg);
}
void CDChanger::handler_RequestPlayback(AVCLanMessage messageIn) {
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportPlayback;
    responseMsg.setOperands({0x01, 0x10, 0x01, 0x69, 0x00, 0x01, 0x00, 0x00, 0x94, 0x00});
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
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportToc;
    responseMsg.setOperands({0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01});
	std::invoke(sendMessageCallback, responseMsg);

}
void CDChanger::handler_RequestTrackName(AVCLanMessage messageIn) {
}
