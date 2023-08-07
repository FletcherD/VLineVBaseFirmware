#include "uart.h"

#include "diag/trace.h"
/* USART Driver */
extern ARM_DRIVER_USART Driver_USART0;
extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_USART Driver_USART2;
extern ARM_DRIVER_USART Driver_USART3;

uart* uart::uartInstance[];

uart::uart(uint8_t uartNum, uint32_t baudRate)
{
	uint32_t errNo;
#if (RTE_UART0)
	if(uartNum == 0) {
		USARTdrv = &Driver_USART0;
		errNo = USARTdrv->Initialize(&uart::signalEventUART0);
	}
#endif

#if (RTE_UART1)
	if(uartNum == 1) {
		USARTdrv = &Driver_USART1;
		errNo = USARTdrv->Initialize(&uart::signalEventUART1);
	}
#endif

#if (RTE_UART2)
	if(uartNum == 2) {
		USARTdrv = &Driver_USART2;
		errNo = USARTdrv->Initialize(&uart::signalEventUART2);
	}
#endif

#if (RTE_UART3)
	if(uartNum == 3) {
		USARTdrv = &Driver_USART3;
		errNo = USARTdrv->Initialize(&uart::signalEventUART3);
	}
#endif

/*
	ARM_USART_CAPABILITIES drv_capabilities = USARTdrv->GetCapabilities();
#define CAPABILITY_STR(f) (drv_capabilities.f ? #f "\n" : "")
	trace_printf("UART %d Capabilities: \n%s %s %s %s",
			uartNum,
			CAPABILITY_STR())
*/

	errNo |= USARTdrv->PowerControl(ARM_POWER_FULL);
	errNo |= USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
					ARM_USART_DATA_BITS_8 |
					ARM_USART_PARITY_NONE |
					ARM_USART_STOP_BITS_1 |
					ARM_USART_FLOW_CONTROL_NONE, baudRate);
	errNo |= USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
	errNo |= USARTdrv->Control (ARM_USART_CONTROL_RX, 1);

	uartInstance[uartNum] = this;
}


uint32_t
uart::send(const void* data, uint32_t len)
{
	return USARTdrv->Send(data, len);
}

void
uart::queueSend(SendData thisData)
{
	sendBuf.push(thisData);
	if (sendReady) {
		sendNextBuf();
	}
}

int
uart::printf(const char *format, ...)
{
	va_list ap;
	va_start (ap, format);
	SendData thisData;
	thisData.size = vasprintf((char**)&thisData.data, format, ap);
	va_end (ap);

	queueSend(thisData);

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
uart::signalEventUART0(uint32_t event) {
	uartInstance[0]->signalEvent(event);
}
void
uart::signalEventUART1(uint32_t event) {
	uartInstance[1]->signalEvent(event);
}
void
uart::signalEventUART2(uint32_t event) {
	uartInstance[2]->signalEvent(event);
}
void
uart::signalEventUART3(uint32_t event) {
	uartInstance[3]->signalEvent(event);
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
	if(event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
		if(receiveComplete)
			receiveComplete();
	}
}

uart uartOut(3, 1500000);
