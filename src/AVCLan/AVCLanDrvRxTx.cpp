#include "AVCLanDrvRxTx.h"

AVCLanDrvRxTx* AVCLanDrvRxTx::instance;

AVCLanDrvRxTx::AVCLanDrvRxTx(p_timer timer)
	: AVCLanTx(timer),
	  AVCLanRx(timer),
	  AVCLanDrvBase(timer)
{
	AVCLanDrvRxTx::instance = this;
}

void AVCLanDrvRxTx::onTimerCallback() {
	if(operatingMode == RECEIVE) {
		AVCLanRx::onTimerCallback();
	}
	else if(operatingMode == TRANSMIT) {
		AVCLanTx::onTimerCallback();
	}
}


void AVCLanDrvRxTx::sendMessage(uint8_t* thisMessage, uint32_t thisMessageLengthBits) {
	queueMessage(thisMessage, thisMessageLengthBits);

	if( !AVCLanRx::isBusy() ) {
		operatingMode = TRANSMIT;

	}
}


extern "C" {
void TIMER2_IRQHandler(void) {
	AVCLanDrvRxTx::instance->onTimerCallback();
}
}
