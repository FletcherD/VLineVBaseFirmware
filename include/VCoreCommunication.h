#include "uart.h"
#include "AVCLanDrvRxTx.h"

class VCoreCommunication {
	static AVCLanMsg uartReceiveMsg;

public:
	static void onMessageReceived(AVCLanMsg message);

	static void receiveFromUart();

	static void receiveComplete();

	static uart uartVCore;
};


