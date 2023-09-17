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

	static constexpr Opcode CdInserted 			= 0x50;
	static constexpr Opcode CdRemoved 			= 0x51;
	static constexpr Opcode RequestStatus	 	= 0xE0; // TODO figure this out
	static constexpr Opcode ReportStatus	 	= 0xF0; // TODO figure this out
	static constexpr Opcode RequestPlayback 	= 0xE1;
	static constexpr Opcode ReportPlayback 		= 0xF1;
	static constexpr Opcode RequestPlayback2 	= 0xE2;
	static constexpr Opcode ReportPlayback2 	= 0xF2;
	static constexpr Opcode RequestLoader 		= 0xE3;
	static constexpr Opcode ReportLoader 		= 0xF3;
	static constexpr Opcode RequestLoader2 		= 0xE4;
	static constexpr Opcode ReportLoader2 		= 0xF4;
	static constexpr Opcode RequestToc 			= 0xE9;
	static constexpr Opcode ReportToc 			= 0xF9;
	static constexpr Opcode RequestStatusFA		= 0xEA; // TODO what is this?
	static constexpr Opcode ReportStatusFA 		= 0xFA; // TODO what is this?
	static constexpr Opcode RequestTrackName	= 0xED;
	static constexpr Opcode ReportTrackName		= 0xFD;

	CDChanger();

	void handler_RequestStatus(AVCLanMessage);
	void handler_RequestPlayback(AVCLanMessage);
	void handler_RequestPlayback2(AVCLanMessage);
	void handler_RequestLoader(AVCLanMessage);
	void handler_RequestLoader2(AVCLanMessage);
	void handler_RequestToc(AVCLanMessage);
	void handler_RequestTrackName(AVCLanMessage);
	void handler_RequestStatusFA(AVCLanMessage);

	void handler_Function07Request(AVCLanMessage);

	uint8_t playbackSeconds = 0;
	void sendPlayback();
	bool haveSentTOC = false;
	void sendTOC();
	void sendCDInserted();
};

#endif /* CDCHANGER_H_ */
