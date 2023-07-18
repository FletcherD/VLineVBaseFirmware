#include "uart.h"

#include "diag/trace.h"
/* USART Driver */
extern ARM_DRIVER_USART Driver_USART0;

void
uart::callback(uint32_t event)
{
}

uint32_t
uart::start()
{
	USARTdrv = &Driver_USART0;

	ARM_USART_CAPABILITIES drv_capabilities = USARTdrv->GetCapabilities();
	trace_printf("event_tx_complete: %d\n", drv_capabilities.event_tx_complete);
	/*Initialize the USART driver */
	uint32_t errNo = USARTdrv->Initialize(&uart::callback);
	errNo |= USARTdrv->PowerControl(ARM_POWER_FULL);
	errNo |= USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
					ARM_USART_DATA_BITS_8 |
					ARM_USART_PARITY_NONE |
					ARM_USART_STOP_BITS_1 |
					ARM_USART_FLOW_CONTROL_NONE, 1500000);
	errNo |= USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
	errNo |= USARTdrv->Control (ARM_USART_CONTROL_RX, 1);
	return errNo;
}


uint32_t
uart::send(const void* data, uint32_t len)
{
	return USARTdrv->Send(data, len);
}

int
uart::printf(const char *format, ...)
{
	static constexpr uint32_t bufSize = 128;
	static char buf[bufSize];

	int ret;
	va_list ap;
	va_start (ap, format);
	ret = vsnprintf (buf, sizeof(buf), format, ap);
	if (ret > 0) {
		send(buf, ret);
		//send("\r\n",2);
	}
	va_end (ap);
	return ret;
}

uart uartOut;
