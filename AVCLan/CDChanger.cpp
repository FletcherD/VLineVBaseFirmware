/*
 * CDChanger.cpp
 *
 *  Created on: Aug 18, 2023
 *      Author: fletcher
 */

#include "CDChanger.h"

CDChanger::CDChanger()
	: Device(address, devices)
{
	messageHandlerMap[RequestStatus] 	= (MessageHandler)&CDChanger::handler_RequestStatus;
	messageHandlerMap[RequestPlayback] 	= (MessageHandler)&CDChanger::handler_RequestPlayback;
	messageHandlerMap[RequestPlayback2] = (MessageHandler)&CDChanger::handler_RequestPlayback2;
	messageHandlerMap[RequestLoader] 	= (MessageHandler)&CDChanger::handler_RequestLoader;
	messageHandlerMap[RequestLoader2] 	= (MessageHandler)&CDChanger::handler_RequestLoader2;
	messageHandlerMap[RequestToc] 		= (MessageHandler)&CDChanger::handler_RequestToc;
	messageHandlerMap[RequestTrackName] = (MessageHandler)&CDChanger::handler_RequestTrackName;
}


void CDChanger::handler_RequestStatus(Message messageIn) {
	Message responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportStatus;
    responseMsg.operands = {0x06, 0x13, 0x90, 0xf1, 0x01, 0x03};
	std::invoke(sendMessageCallback, responseMsg);
}
void CDChanger::handler_RequestPlayback(Message messageIn) {
	Message responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportPlayback;
    responseMsg.operands = {0x01, 0x10, 0x01, 0x69, 0x00, 0x01, 0x00, 0x00, 0x94, 0x00};
	std::invoke(sendMessageCallback, responseMsg);

}
void CDChanger::handler_RequestPlayback2(Message messageIn) {
	Message responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportPlayback2;
    responseMsg.operands = {0x00, 0x30, 0x01, 0x01, 0x00, 0x06, 0x00, 0x80};
	std::invoke(sendMessageCallback, responseMsg);

}
void CDChanger::handler_RequestLoader(Message messageIn) {
}
void CDChanger::handler_RequestLoader2(Message messageIn) {
	Message responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportLoader2;
    responseMsg.operands = {0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x02};
	std::invoke(sendMessageCallback, responseMsg);

}
void CDChanger::handler_RequestToc(Message messageIn) {
	Message responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportToc;
    responseMsg.operands = {0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};
	std::invoke(sendMessageCallback, responseMsg);

}
void CDChanger::handler_RequestTrackName(Message messageIn) {
}
