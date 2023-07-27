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


extern "C" {
void TIMER2_IRQHandler(void) {
	AVCLanDrvRxTx::instance->onTimerCallback();
}
}
