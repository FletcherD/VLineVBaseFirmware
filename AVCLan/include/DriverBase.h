#ifndef AVCLANDRV_H_
#define AVCLANDRV_H_

#include "cmsis_device.h"
extern "C" {
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
}
#include "timer.h"
#include "uart.h"
#include "util.h"

#include "IEBusMessage.h"

class DriverBase {
	/*
	 * The DriverBase class contains things used by both Rx and Tx,
	 * such as the timing information of bits,
	 * and the GPIO pins used for RX and TX.
	 */
public:
	typedef uint32_t Time;

	uint32_t eTime[512];
	uint32_t eTimeI = 0;

protected:
	p_timer timer;

	// 40.690104167 μs
	static constexpr float T_Bit_uS = 256e6 / (0x600000);

	// Times in timer ticks
	const uint32_t T_StartBit	= timer.uS(	170	);
	const uint32_t T_Bit		= timer.uS(	T_Bit_uS );
	const uint32_t T_Bit_1		= timer.uS(	T_Bit_uS / 2. );
	const uint32_t T_Bit_0		= timer.uS(	4.* T_Bit_uS / 5. );
	const uint32_t T_BitMeasure	= (T_Bit_1 + T_Bit_0) / 2;

	const uint32_t T_Rising_1	= (T_Bit - T_Bit_0) + T_Bit_1;
	const uint32_t T_Rising_0	= (T_Bit - T_Bit_1) + T_Bit_0;
	const uint32_t T_RisingMeasure_1 = (T_Bit + T_Rising_1) / 2;
	const uint32_t T_RisingMeasure_0 = (T_Bit + T_Rising_0) / 2;
	const uint32_t T_Ack		= timer.uS(10);

	const uint32_t T_TxWait		= timer.uS(	5000 );

	// ------------------------

	PinCfgType AvcRxPin = {
		.PortNum 	=  	0,
		.PinNum 	= 	4,
		// Timer Capture function
		.FuncNum 	= 	PIN_FUNC_3,
		.PinMode 	= 	PIN_PINMODE_PULLUP,
		.OpenDrain 	= 	PIN_PINMODE_NORMAL,
		.GpioDir 	= 	GPIO_DIR_INPUT
	};
	PinCfgType AvcTxPin = {
		.PortNum 	=  	0,
		.PinNum 	= 	5,
		.FuncNum 	= 	PIN_FUNC_0,
		.PinMode 	= 	PIN_PINMODE_TRISTATE,
		.OpenDrain 	= 	PIN_PINMODE_OPENDRAIN,
		.GpioDir 	= 	GPIO_DIR_OUTPUT
	};
	PinCfgType AvcStandbyPin = {
		.PortNum 	=  	2,
		.PinNum 	= 	5,
		.FuncNum 	= 	PIN_FUNC_0,
		.PinMode 	= 	PIN_PINMODE_TRISTATE,
		.OpenDrain 	= 	PIN_PINMODE_OPENDRAIN,
		.GpioDir 	= 	GPIO_DIR_OUTPUT
	};
	PinCfgType AvcEnablePin = {
		.PortNum 	=  	1,
		.PinNum 	= 	17,
		.FuncNum 	= 	PIN_FUNC_0,
		.PinMode 	= 	PIN_PINMODE_TRISTATE,
		.OpenDrain 	= 	PIN_PINMODE_NORMAL,
		.GpioDir 	= 	GPIO_DIR_OUTPUT
	};

	bool getRxState();
	void setTxState(bool isOn);

	void setStandby(bool isOn);
	void setEnabled(bool isOn);

	// -------------------

	std::shared_ptr<IEBusMessage> curMessage;
	const IEBusMessageField* curField;
	uint32_t curBit;
	bool curBitValue;
	bool curParity;

	enum AVCLanMode {
		IDLE,
		RECEIVE,
		TRANSMIT,
	};
	AVCLanMode operatingMode = IDLE;

	Time canTxTime;

	void sendMessage();
	void endTransmit();
	void startReceive();
	void endReceive();
	void startIdle();

	void nextBit();

	void resetBuffer();

	virtual void prepareTransmit() = 0;
	virtual void startTransmit() = 0;

	virtual void collisionRecover() = 0;

public:
	DriverBase(p_timer);
};

#endif
