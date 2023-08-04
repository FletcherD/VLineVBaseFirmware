#ifndef AVCLANDRVRXTX_H_
#define AVCLANDRVRXTX_H_

#include "AVCLanRx.h"
#include "AVCLanTx.h"

#include <functional>

class AVCLanDrvRxTx : public AVCLanRx, AVCLanTx
{
	void startTransmit();
	void endTransmit();
	void startReceive();
	void endReceive();
	void startIdle();

	void messageReceived(AVCLanMsg);

	enum AVCLanMode {
		RECEIVE,
		TRANSMIT,
		IDLE
	};
	AVCLanMode operatingMode = IDLE;

public:
	static AVCLanDrvRxTx* instance;

	AVCLanDrvRxTx(p_timer timer);
	~AVCLanDrvRxTx();

	std::function<void(AVCLanMsg)> messageReceivedCallback;

	void onTimerCallback();

	void sendMessage(AVCLanMsg);
};

#endif
