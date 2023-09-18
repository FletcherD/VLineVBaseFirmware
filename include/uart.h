#ifndef UART_H_
#define UART_H_

extern "C" {
#include "../driver/Config/RTE_Device.h"
#include "Driver_USART.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
}
#include <functional>
#include <queue>

class uart
{
public:
	std::function<void(void)> receiveComplete;

private:
	static void	signalEventUART0(uint32_t event);
	static void	signalEventUART1(uint32_t event);
	static void	signalEventUART2(uint32_t event);
	static void	signalEventUART3(uint32_t event);
	void signalEvent(uint32_t event);
	static uart* uartInstance[4];

	std::queue<char> sendBuf;
	bool isEnabled = true;

	void sendNextByte();

public:
	bool sendReady = true;

	ARM_DRIVER_USART * USARTdrv;
	uart(uint8_t uartNum, uint32_t baudRate);

	//void queueSend(SendData);

	size_t printf(const char* format, ...);

	void setEnabled(bool isEnabled);
};

extern uart uartOut;

#endif
