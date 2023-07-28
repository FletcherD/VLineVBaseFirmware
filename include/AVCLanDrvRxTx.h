#include "AVCLanRx.h"
#include "AVCLanTx.h"


class AVCLanDrvRxTx : public AVCLanRx, AVCLanTx
{
	void txEnd();
	void rxEnd();

	void startTransmit();
	void startReceive();
	void startIdle();

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

	void onTimerCallback();

	void sendMessage(AVCLanMsg);
};


