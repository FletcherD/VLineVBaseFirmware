#include "AVCLanDrv.h"
#include "diag/trace.h"

extern "C" {
void TIMER2_IRQHandler(void) {
	AVCLanDrv::instance->onTimerCallback();
}
// GPIO interrupt
void EINT3_IRQHandler(void) {
	AVCLanDrv::instance->onGpioCallback();
}
}

void AVCLanDrv::onTimerCallback() {
	uint32_t gpioVal = GPIO_PortRead(AVC_RX_PIN.Portnum);
	uartOut.printf("GPIO: %08x %d\r\n", gpioVal, timer.getBigTicks());
	timer.clearInterrupt();
}
void AVCLanDrv::onGpioCallback() {
	uint32_t gpioVal = GPIO_PortRead(AVC_RX_PIN.Portnum);
	uartOut.printf("GPIO: %08x %d\r\n", gpioVal, timer.getBigTicks());
}

bool AVCLanDrv::readBit() {
	// Measure the pin at a specific time past the falling edge
	while(isInputSet());
	while(isInputClear());
	timer.sleep(T_BitMeasure);
	bool bit = isInputClear();
	uartOut.send((bit?"1":"0"),1);
	return bit;
}

uint32_t AVCLanDrv::readBits(uint8_t n) {
	uint32_t val = 0;
	for(uint8_t i = 0; i != n; i++) {
		bool bit = readBit();
		if (bit) {
			val |= (0x1 << i);
		}
	}
	return val;
}

void AVCLanDrv::readMessage() {
	while(isInputClear());
	timer.reset();
	while(isInputSet());
	// TODO: validate start bit time

	while(1) {
		readBit();
	}

	bool parityBit;
	bool ackBit;

	AvcTransmissionMode broadcastBit = readBit() ? AVC_MSG_DIRECT : AVC_MSG_BROADCAST;

	uint16_t masterAddress = readBits(12);
	parityBit = readBit();
	ackBit = readBit();
	uint16_t slaveAddress = readBits(12);
	parityBit = readBit();
	ackBit = readBit();
	uint8_t controlField = readBits(4);
	parityBit = readBit();
	ackBit = readBit();
	uint8_t dataLength = readBits(8);
	parityBit = readBit();
	ackBit = readBit();
	uint8_t data[dataLength];
	for(uint8_t i = 0; i < dataLength; i++) {
		data[i] = readBits(8);
		parityBit = readBit();
		ackBit = readBit();
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

	LPC_GPIO(0)->DIR = 0x00400320;
	LPC_GPIO(1)->DIR = 0x7ffbc113;
	LPC_GPIO(2)->DIR = 0x00003964;
	LPC_GPIO(3)->DIR = 0x06000000;
	LPC_GPIO(4)->DIR = 0x10000000;
	LPC_GPIO(0)->PIN = 0x387b0c3f;
	LPC_GPIO(1)->PIN = 0x18354703;
	LPC_GPIO(2)->PIN = 0x00002759;
	LPC_GPIO(3)->PIN = 0x06000000;
	LPC_GPIO(4)->PIN = 0x00000000;
	LPC_PINCON->PINMODE0 = 0xffaaaaaa;
	LPC_PINCON->PINMODE1 = 0xeaaeaabf;
	LPC_PINCON->PINMODE2 = 0xaff2fefa;
	LPC_PINCON->PINMODE3 = 0xeaaaaaba;
	LPC_PINCON->PINMODE4 = 0x0aba2aaa;
	LPC_PINCON->PINMODE7 = 0x00280000;
	LPC_PINCON->PINMODE9 = 0x0b000000;
	LPC_PINCON->PINMODE_OD0 = 0x18580c20;
	LPC_PINCON->PINMODE_OD1 = 0x00100100;
	LPC_PINCON->PINMODE_OD2 = 0x00002020;
	LPC_PINCON->PINMODE_OD3 = 0x04000000;
	LPC_PINCON->PINMODE_OD4 = 0x00000000;

	pinConfigure(AVC_RX_PIN);
	pinConfigure(AVC_TX_PIN);
	pinConfigure(AVC_STB_PIN);
	pinConfigure(AVC_EN_PIN);
	pinConfigure(AVC_ERR_PIN);

	timer.setupCaptureInterrupt();

	setStandby(false);
	setEnabled(true);
	setTx(false);

	while(true) {
		  uartOut.printf("Idle\r\n");
		  timer.sleep(100000);
	}
/*
	uint32_t lastTime = 0;
	uint32_t tBuf[1024];
	uint32_t i,t = 0;
	while(isInputClear());
	timer.reset();
	while(1) {
		i=0;
		while(i<1024) {
			while(isInputSet());
			t = timer.getTicks();
			tBuf[i++] = t-lastTime;
			lastTime=t;
			while(isInputClear());
			t = timer.getTicks();
			tBuf[i++] = t-lastTime;
			lastTime=t;
		}
		for(i=0; i<1024; i++)
			uartOut.printf("%d\r\n", tBuf[i]);
	}
	*/
}

AVCLanDrv* AVCLanDrv::instance;
