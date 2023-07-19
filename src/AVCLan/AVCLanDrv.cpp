#include "AVCLanDrv.h"
#include "diag/trace.h"

extern "C" {
void TIMER2_IRQHandler(void) {
	AVCLanDrv::instance->onTimerCallback();
}
}

void AVCLanDrv::onTimerCallback() {
	uint32_t interruptType = timer.lpcTimer->IR;
	timer.clearInterrupt();
	timer.updateTimer(T_Timeout);

	InputEvent event;
	if(interruptType & 1UL)
	{
		event.type = TIMEOUT;
	} else {
		uint32_t eventTime = timer.lpcTimer->CR0;
		uint32_t rxIn = GPIO_PortRead(AVC_RX_PIN.Portnum);

		event.time = eventTime - lastEventTime;
		lastEventTime = eventTime;
		event.type = (rxIn & (1UL << AVC_RX_PIN.Pinnum)) ? FALLING_EDGE : RISING_EDGE;
	}

	(this->*state)(event);
}


void AVCLanDrv::state_Idle(InputEvent e) {
	if(e.type == FALLING_EDGE) {
		state = &AVCLanDrv::state_StartBit;
	}
}

void AVCLanDrv::state_StartBit(InputEvent e) {
	if(e.type == RISING_EDGE) {
		if(e.time < 10) {
			// Spurious
			state = &AVCLanDrv::state_Idle;
			return;
		}
		uartOut.printf("StartBitLen: %d\r\n", e.time);
		bufPos = 0;
		state = &AVCLanDrv::state_WaitForBit;
	} else if (e.type == TIMEOUT) {
		uartOut.printf("Timeout during StartBit \r\n", e.time);
		state = &AVCLanDrv::state_Idle;
	}
}
void AVCLanDrv::state_WaitForBit(InputEvent e) {
	if(e.type == FALLING_EDGE) {
		state = &AVCLanDrv::state_MeasureBit;
	} else if (e.type == TIMEOUT) {
		uartOut.printf("Timeout during WaitForBit \r\n", e.time);
		state = &AVCLanDrv::state_Idle;
	}
}
void AVCLanDrv::state_MeasureBit(InputEvent e) {
	if(e.type == RISING_EDGE) {
		uartOut.printf("BitLen: %d\r\n", e.time);
		state = &AVCLanDrv::state_WaitForBit;
	} else if (e.type == TIMEOUT) {
		uartOut.printf("Timeout during MeasureBit \r\n", e.time);
		state = &AVCLanDrv::state_Idle;
	}
}

void AVCLanDrv::receiveBit(bool bit) {
	if(bit) {
		uint8_t whichByte = (bufPos / 8);
		uint8_t whichBit  = (bufPos % 8);
		messageBuf[whichByte] |= (1<<whichBit);
	}
}

#define LPC_GPIO(n)             ((LPC_GPIO_TypeDef *)(LPC_GPIO0_BASE + 0x00020*n))
#define DIR  FIODIR
#define SET  FIOSET
#define CLR  FIOCLR
#define PIN  FIOPIN
#define MASK FIOMASK
// AVCLan driver & timer2 init,
void AVCLanDrv::begin (){
	AVCLanDrv::instance = this;

	state = &AVCLanDrv::state_Idle;



	pinConfigure(AVC_RX_PIN);
	pinConfigure(AVC_TX_PIN);
	pinConfigure(AVC_STB_PIN);
	pinConfigure(AVC_EN_PIN);
	pinConfigure(AVC_ERR_PIN);

	timer.setupCaptureInterrupt();
	timer.setupTimerInterrupt(T_Timeout);

	setStandby(false);
	setEnabled(true);
	setTx(false);
}

AVCLanDrv* AVCLanDrv::instance;
