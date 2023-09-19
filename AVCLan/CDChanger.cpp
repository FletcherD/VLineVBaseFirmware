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
	addHandler(RequestStatus, (MessageHandler)&CDChanger::handler_RequestStatus);
	addHandler(RequestStatusPlayback, (MessageHandler) & CDChanger::handler_RequestStatusPlayback);
	addHandler(RequestStatusPlayback2, (MessageHandler) & CDChanger::handler_RequestStatusPlayback2);
	addHandler(RequestStatusLoader, (MessageHandler) & CDChanger::handler_RequestStatusLoader);
	addHandler(RequestStatusLoader2, (MessageHandler) & CDChanger::handler_RequestStatusLoader2);
	addHandler(RequestStatusToc, (MessageHandler) & CDChanger::handler_RequestStatusToc);
	addHandler(RequestStatusFA, (MessageHandler)&CDChanger::handler_RequestStatusFA);
	addHandler(RequestStatusTrackName, (MessageHandler) & CDChanger::handler_RequestStatusTrackName);

	addHandler(Function07Request, (MessageHandler)&CDChanger::handler_Function07Request);

	addHandler(EnableFunctionRequest, (MessageHandler)&CDChanger::handler_EnableFunctionRequest);
	addHandler(DisableFunctionRequest, (MessageHandler)&CDChanger::handler_DisableFunctionRequest);

	addHandler(PlayRequest, (MessageHandler)&CDChanger::handler_PlayRequest);
}


void CDChanger::handler_RequestStatus(AVCLanMessage messageIn) {
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportStatus;
    responseMsg.setOperands({0x06, 0x13, 0x90, 0xf1, 0x01, 0x03});
	sendMessage(responseMsg);
}

void CDChanger::handler_RequestStatusPlayback(AVCLanMessage messageIn) {
	sendStatusPlayback();
}

void CDChanger::sendStatusPlayback() {
	sendMessage(AVCLanMessage(
		BROADCAST,
		address,
		0x1ff,
		0x43,
		0x31,
		ReportStatusPlayback,
		{0x01, 0x10, 0x01, 0x77, 0x00, playbackSeconds, 0x00, 0x00, 0x94, 0x00} ) );
	playbackSeconds++;
}

void CDChanger::handler_RequestStatusPlayback2(AVCLanMessage messageIn) {
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportStatusPlayback2;
    responseMsg.setOperands({0x00, 0x30, 0x01, 0x01, 0x00, 0x06, 0x00, 0x80});
	sendMessage(responseMsg);
}

void CDChanger::handler_RequestStatusLoader(AVCLanMessage messageIn) {
}

void CDChanger::handler_RequestStatusLoader2(AVCLanMessage messageIn) {
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
    responseMsg.opcode = ReportStatusLoader2;
    responseMsg.setOperands({0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x02});
	sendMessage(responseMsg);
}

void CDChanger::handler_RequestStatusToc(AVCLanMessage messageIn) {
	sendStatusTOC();
}
void CDChanger::sendStatusTOC() {
	sendMessage(AVCLanMessage(
		BROADCAST,
		address,
		0x1ff,
		0x43,
		0x25,
		ReportStatusToc,
		{0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01}) );
}

void CDChanger::handler_RequestStatusTrackName(AVCLanMessage messageIn) {
}

void CDChanger::handler_EnableFunctionRequest(AVCLanMessage messageIn)
{
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = EnableFunctionResponse;
	responseMsg.setOperands(messageIn.getOperands());
	sendMessage(responseMsg);

	if(messageIn.dstFunction == 0x43) {
		AVCLanMessage responseMsgCd1(IEBusMessage{
			.broadcast 		= UNICAST,
			.masterAddress 	= address,
			.slaveAddress 	= 0x110,
		});
		responseMsgCd1.srcFunction = 0x43;
		responseMsgCd1.dstFunction = 0x12;
		responseMsgCd1.opcode = CdInserted;
		responseMsgCd1.setOperands({0x0a, 0x01});
		sendMessage(responseMsgCd1);

		sendStatusTOC();
	}
}

void CDChanger::handler_DisableFunctionRequest(AVCLanMessage messageIn)
{
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = DisableFunctionResponse;
	responseMsg.setOperands(messageIn.getOperands());
	sendMessage(responseMsg);
}

void CDChanger::handler_RequestStatusFA(AVCLanMessage messageIn) {
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = ReportStatusFA;
	responseMsg.setOperands({0x01, 0x00, 0x77, 0x77, 0x59, 0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01});
	sendMessage(responseMsg);
}

void CDChanger::handler_Function07Request(AVCLanMessage messageIn)
{
	AVCLanMessage responseMsg = createResponseMessage(messageIn);
	responseMsg.opcode = Function07Response;
	responseMsg.setOperands({});
	sendMessage(responseMsg);

	AVCLanMessage responseMsgB0 = createResponseMessage(messageIn);
	responseMsgB0.opcode = StatusB0;
	responseMsgB0.dstFunction = 0x12;
	responseMsgB0.setOperands({});
	sendMessage(responseMsgB0);
}

void CDChanger::handler_PlayRequest(AVCLanMessage messageIn) {
	sendMessage(AVCLanMessage(
		UNICAST,
		address,
		0x110,
		0xb0,
		0x12,
		CdInserted,
		{0x0a}) );

	sendMessage(AVCLanMessage(
		UNICAST,
		address,
		0x110,
		0x43,
		0x12,
		CdInserted,
		{0x01}) );
}

