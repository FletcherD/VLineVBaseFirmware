extern "C" {
#include "Driver_USART.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
}
#include <functional>
#include <queue>
#include <vector>

class uart
{
private:
	static void
	signalEventStatic(uint32_t event);
	void
	signalEvent(uint32_t event);

	struct SendData {
		char* data;
		uint32_t size;
	};
	std::queue<SendData> sendBuf;
	static constexpr uint32_t sendBufSize = 32;


	void
	sendNextBuf();

public:
	bool sendReady = true;

	ARM_DRIVER_USART * USARTdrv;
	uint32_t
	start();

	uint32_t
	send(const void* data, uint32_t len);

	int
	printf(const char* format, ...);
};

extern uart uartOut;
