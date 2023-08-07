#ifndef AVCLANDRVRXTX_H_
#define AVCLANDRVRXTX_H_

#include "AVCLanRx.h"
#include "AVCLanTx.h"

#include <functional>
#include <queue>

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

public:
	AVCLanMode operatingMode = IDLE;
	static AVCLanDrvRxTx* instance;

	AVCLanDrvRxTx(p_timer timer);
	~AVCLanDrvRxTx();

	std::queue<AVCLanMsg> receiveQueue;

	void onTimerCallback();

	void sendMessage(AVCLanMsg);
};

#endif
