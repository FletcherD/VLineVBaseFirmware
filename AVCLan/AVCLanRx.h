#ifndef AVCLANRX_H_
#define AVCLANRX_H_

#include "cmsis_device.h"
extern "C" {
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
}
#include "timer.h"
#include "uart.h"
#include "util.h"

#include "AVCLanDrv.h"
#include "AVCLanMsg.h"

class AVCLanRx : public virtual AVCLanDrvBase {
	private:
		typedef uint32_t Time;
		enum InputType {
			RISING_EDGE,
			FALLING_EDGE
		};
		struct InputEvent {
			Time time;
			InputType type;
		};
		Time lastEventTime;

		// AVC-LAN RX state machine
		typedef void (AVCLanRx::*State)(InputEvent);
		State state;

		void state_Idle(InputEvent i);
		void state_StartBit(InputEvent i);
		void state_WaitForBit(InputEvent i);
		void state_MeasureBit(InputEvent i);

		// ------------------------

		AVCLanMsg thisMsg;
		uint32_t receiveBitPos;

		void receiveBit(bool bit);
		void resetBuffer();
		void messageEnd();

		void onBitError();

		virtual void endReceive() {};
		virtual void messageReceived(AVCLanMsg) {};

		// Because we have a very strict time budget,
		// calculate the length on the fly
		// so AVCLanRx knows when it is done
		uint8_t msgDataLength = 0;

	public:
		uint32_t bitErrorCount = 0;
		uint32_t totalMsgCount = 0;

		size_t longestMsg = 0;

		AVCLanRx(p_timer);
		virtual ~AVCLanRx() {};

		void onTimerCallback();

};

#endif
