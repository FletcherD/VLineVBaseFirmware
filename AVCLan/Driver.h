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
		RECEIVE,
		TRANSMIT,
		IDLE
	};

public:
	AVCLanMode operatingMode = IDLE;
	static Driver* instance;

	Driver(p_timer timer);
	~Driver();

	std::queue<MessageRaw> receiveQueue;

	void onTimerCallback();

	void sendMessage(MessageRaw);
};

#endif
