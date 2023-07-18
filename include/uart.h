extern "C" {
#include "Driver_USART.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
}
#include <functional>

class uart
{
private:

	static void
	callback(uint32_t event);

public:
	ARM_DRIVER_USART * USARTdrv;
	uint32_t
	start();

	uint32_t
	send(const void* data, uint32_t len);

	int
	printf(const char* format, ...);
};

extern uart uartOut;
