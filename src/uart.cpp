#include "uart.h"

#include "diag/trace.h"
/* USART Driver */
extern ARM_DRIVER_USART Driver_USART0;


uint32_t
uart::start()
{
	USARTdrv = &Driver_USART0;

	ARM_USART_CAPABILITIES drv_capabilities = USARTdrv->GetCapabilities();

	/*Initialize the USART driver */
	uint32_t errNo = USARTdrv->Initialize(&uart::signalEventStatic);
	errNo |= USARTdrv->PowerControl(ARM_POWER_FULL);
	errNo |= USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
					ARM_USART_DATA_BITS_8 |
					ARM_USART_PARITY_NONE |
					ARM_USART_STOP_BITS_1 |
					ARM_USART_FLOW_CONTROL_NONE, 921600);
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
	int size;
	va_list ap;
	va_start (ap, format);

	va_list apCopy;
	va_copy(apCopy, ap);
	size_t strSize = vsnprintf(NULL, 0, format, apCopy);
	va_end(apCopy);

	SendData thisData;
	thisData.data = new char[strSize+1];
	thisData.size = vsnprintf(thisData.data, strSize+1, format, ap);
	sendBuf.push(thisData);

	if (sendReady) {
		sendNextBuf();
	}

	va_end (ap);
	return thisData.size;
}

void
uart::sendNextBuf()
{
	sendReady = false;
	SendData thisData = sendBuf.front();
	send(thisData.data, thisData.size);
}

void
uart::signalEventStatic(uint32_t event) {
	uartOut.signalEvent(event);
}

void
uart::signalEvent(uint32_t event)
{
	if(event & ARM_USART_EVENT_SEND_COMPLETE) {
		delete sendBuf.front().data;
		sendBuf.pop();

		if(sendBuf.empty()) {
			sendReady = true;
		} else {
			sendNextBuf();
		}
	}
}

uart uartOut;
