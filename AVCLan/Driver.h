#ifndef DRIVER_H_
#define DRIVER_H_

#include <DriverRx.h>
#include <DriverTx.h>
#include <functional>
#include <queue>

class Driver : public DriverRx, DriverTx
{
	void startTransmit();
	void endTransmit();
	void startReceive();
	void endReceive();
	void startIdle();

	void messageReceived(MessageRaw);

	enum AVCLanMode {
		IDLE,
		RECEIVE,
		TRANSMIT,
	};

public:
	AVCLanMode operatingMode = IDLE;
	static Driver* instance;

	Driver(p_timer timer);
	~Driver();

	std::queue<MessageRaw> receiveQueue;

	/* All receive and transmit functions use the on-chip timer peripheral.
	 * To receive bits, we use the capture function;
	 * the timer will set a capture register with the current time,
	 * and call an interrupt, when the line level changes. This lets us
	 * time the interval between line transitions.
	 * To transmit bits, we use the basic timer match registers
	 * to set the line at the correct time intervals.
	 */

	void onTimerCallback();

	void sendMessage(MessageRaw);
};

#endif
