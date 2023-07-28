#include "AVCLanTx.h"
#include "AVCLanRx.h"


class AVCLanDrvRxTx : public AVCLanRx, AVCLanTx
{
public:
	static AVCLanDrvRxTx* instance;

	AVCLanDrvRxTx(p_timer timer);

	void onTimerCallback();

	void sendMessage(uint8_t* thisMessage, uint32_t thisMessageLength);
};


