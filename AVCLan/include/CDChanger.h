/*
 * CDChanger.h
 *
 *  Created on: Aug 18, 2023
 *      Author: fletcher
 */

#ifndef CDCHANGER_H_
#define CDCHANGER_H_

#include "Device.h"

class CDChanger : public Device {
public:
	static constexpr Function ChChangerFunction = 0x43;
	static constexpr Function UnknownAudioFunction = 0xb0;

	static constexpr Opcode CdInserted 			= 0x50;
	static constexpr Opcode CdRemoved 			= 0x51;
	static constexpr Opcode PlayRequest 		= 0x80;
	static constexpr Opcode RequestStatus	 	= 0xE0; // TODO figure this out
	static constexpr Opcode ReportStatus	 	= 0xF0; // TODO figure this out
	static constexpr Opcode RequestStatusPlayback 	= 0xE1;
	static constexpr Opcode ReportStatusPlayback 		= 0xF1;
	static constexpr Opcode RequestStatusPlayback2 	= 0xE2;
	static constexpr Opcode ReportStatusPlayback2 	= 0xF2;
	static constexpr Opcode RequestStatusLoader 		= 0xE3;
	static constexpr Opcode ReportStatusLoader 		= 0xF3;
	static constexpr Opcode RequestStatusLoader2 		= 0xE4;
	static constexpr Opcode ReportStatusLoader2 		= 0xF4;
	static constexpr Opcode RequestStatusToc 			= 0xE9;
	static constexpr Opcode ReportStatusToc 			= 0xF9;
	static constexpr Opcode RequestStatusFA		= 0xEA; // TODO what is this?
	static constexpr Opcode ReportStatusFA 		= 0xFA; // TODO what is this?
	static constexpr Opcode RequestStatusTrackName	= 0xED;
	static constexpr Opcode ReportStatusTrackName		= 0xFD;

	CDChanger();

	void handler_RequestStatus(AVCLanMessage);
	void handler_RequestStatusPlayback(AVCLanMessage messageIn);
	void handler_RequestStatusPlayback2(AVCLanMessage messageIn);
	void handler_RequestStatusLoader(AVCLanMessage messageIn);
	void handler_RequestStatusLoader2(AVCLanMessage messageIn);
	void handler_RequestStatusToc(AVCLanMessage messageIn);
	void handler_RequestStatusTrackName(AVCLanMessage messageIn);
	void handler_RequestStatusFA(AVCLanMessage);

	void handler_Function07Request(AVCLanMessage);

	void handler_EnableFunctionRequest(AVCLanMessage);
	void handler_DisableFunctionRequest(AVCLanMessage);

	void handler_PlayRequest(AVCLanMessage);

	uint8_t playbackSeconds = 0;
	void sendStatusPlayback();
	bool haveSentTOC = false;
	void sendStatusTOC();
	void sendCDInserted();
};

#endif /* CDCHANGER_H_ */
