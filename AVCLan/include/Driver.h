#ifndef DRIVER_H_
#define DRIVER_H_

#include <DriverRx.h>
#include <DriverTx.h>
#include <functional>
#include <queue>

class Driver : public DriverRx, DriverTx
{
	/* This Driver class derives from both the DriverRX and DriverTX class,
	 * and coordinates both. When the driver is in the IDLE state,
	 * we are free to start transmitting a message.
	 * If a transition on the RX line is detected, we go into the RECEIVE state
	 * until the message is complete.
	 * If we request to send a message while the driver is not IDLE,
	 * it goes into a queue until we're ready to send it.
	 */

	void startTransmit();
	void endTransmit();
	void startReceive();
	void endReceive();
	void startIdle();

	void messageReceived(const std::shared_ptr<IEBusMessage>&);

	Time canTxTime;

public:

	enum AVCLanMode {
		IDLE,
		RECEIVE,
		TRANSMIT,
	};
	AVCLanMode operatingMode = IDLE;
	static Driver* instance;

	Driver(p_timer timer);
	~Driver();

	std::queue<std::shared_ptr<IEBusMessage>> receiveQueue;

	/* All receive and transmit functions use the on-chip timer peripheral.
	 * To receive bits, we use the capture function;
	 * the timer will set a capture register with the current time,
	 * and call an interrupt, when the line level changes. This lets us
	 * time the interval between line transitions.
	 * To transmit bits, we use the basic timer match registers
	 * to set the line at the correct time intervals.
	 */

	void onTimerCallback();

	void sendMessage(const std::shared_ptr<IEBusMessage>&);

	void poll();
};

#endif
