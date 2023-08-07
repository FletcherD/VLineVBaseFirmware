#include "uart.h"
#include "AVCLanDrvRxTx.h"

class VCoreCommunication {
	static AVCLanMsg uartReceiveMsg;

public:
	static void onMessageReceived(AVCLanMsg message);

	static void startUartReceive();

	static void uartReceiveComplete();

	static uart uartVCore;
};


