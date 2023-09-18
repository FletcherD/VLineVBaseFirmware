#include "uart.h"
#include "Driver.h"

class VCoreCommunication {
	static constexpr size_t uartReceiveBufSize = 128;
	static uint8_t uartReceiveBuf[uartReceiveBufSize];
	static uint8_t* uartReceiveByte;

public:
	static void onMessageReceived(const std::shared_ptr<IEBusMessage>&);

	static void startUartReceive();

	static void uartReceiveComplete();

	static uart uartVCore;
};


