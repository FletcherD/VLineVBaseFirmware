/*
  AVCLanDrv.cpp - AVCLan Library for 'duino / Wiring
  Created by Kochetkov Aleksey, 04.08.2010
  Version 0.3.1
*/

#include "AVCLanDrv.h"
#include "BuffSerial.h"

#define LPC_GPIO(n)             ((LPC_GPIO_TypeDef *)(LPC_GPIO0_BASE + 0x00020*n))
#define DIR  FIODIR
#define SET  FIOSET
#define CLR  FIOCLR
#define PIN  FIOPIN
#define MASK FIOMASK
// AVCLan driver & timer2 init, 
void AVCLanDrv::begin (){
	LPC_GPIO(0)->DIR = 0x00448320;
	LPC_GPIO(1)->DIR = 0x7ffbc313;
	LPC_GPIO(2)->DIR = 0x000039e5;
	LPC_GPIO(3)->DIR = 0x06000000;
	LPC_GPIO(4)->DIR = 0x10000000;
	LPC_GPIO(0)->PIN = 0x3c7f8c3f;
	LPC_GPIO(1)->PIN = 0x98174111;
	LPC_GPIO(2)->PIN = 0x00002765;
	LPC_GPIO(3)->PIN = 0x04000000;
	LPC_GPIO(4)->PIN = 0x10000000;
	LPC_PINCON->PINMODE0 = 0x80aaaaaa;
	LPC_PINCON->PINMODE1 = 0x2aa2aaa8;
	LPC_PINCON->PINMODE2 = 0xa00a020a;
	LPC_PINCON->PINMODE3 = 0xaaaaaa8a;
	LPC_PINCON->PINMODE4 = 0x0a8aaaaa;
	LPC_PINCON->PINMODE7 = 0x00280000;
	LPC_PINCON->PINMODE9 = 0x0a000000;
	LPC_PINCON->PINMODE_OD0 = 0x18180c20;
	LPC_PINCON->PINMODE_OD1 = 0x00100100;
	LPC_PINCON->PINMODE_OD2 = 0x00002020;
	LPC_PINCON->PINMODE_OD3 = 0x04000000;
	LPC_PINCON->PINMODE_OD4 = 0x10000000;
	PIN_Configure(AVC_RX_PIN.port, AVC_RX_PIN.num, PIN_FUNC_0, PIN_PINMODE_TRISTATE, PIN_PINMODE_NORMAL);
	GPIO_SetDir(AVC_RX_PIN.port, AVC_RX_PIN.num, GPIO_DIR_INPUT);
	PIN_Configure(AVC_TX_PIN.port, AVC_TX_PIN.num, PIN_FUNC_0, PIN_PINMODE_TRISTATE, PIN_PINMODE_OPENDRAIN);
	GPIO_SetDir(AVC_TX_PIN.port, AVC_TX_PIN.num, GPIO_DIR_OUTPUT);
	PIN_Configure(AVC_STB_PIN.port, AVC_STB_PIN.num, PIN_FUNC_0, PIN_PINMODE_TRISTATE, PIN_PINMODE_OPENDRAIN);
	GPIO_SetDir(AVC_STB_PIN.port, AVC_STB_PIN.num, GPIO_DIR_OUTPUT);
	PIN_Configure(AVC_EN_PIN.port, AVC_EN_PIN.num, PIN_FUNC_0, PIN_PINMODE_TRISTATE, PIN_PINMODE_NORMAL);
	GPIO_SetDir(AVC_EN_PIN.port, AVC_EN_PIN.num, GPIO_DIR_OUTPUT);
	PIN_Configure(AVC_ERR_PIN.port, AVC_ERR_PIN.num, PIN_FUNC_0, PIN_PINMODE_TRISTATE, PIN_PINMODE_NORMAL);
	GPIO_SetDir(AVC_ERR_PIN.port, AVC_ERR_PIN.num, GPIO_DIR_INPUT);
	setStandby(false);
	setEnabled(true);
	setTx(false);

	headAddress   = 0x0000;
	deviceAddress = 0x0000;
	event         = EV_NONE;
	actionID      = ACT_NONE;

	/*
	uint32_t t[256];
	uint32_t ti = 0;
	while(1) {
		while(inputIsClear());
		t[ti++] = timer.getTicks();
		while(inputIsSet());
		t[ti++] = timer.getTicks();
		if(ti == 256)
			break;
	}
	for(ti = 0; ti < 255; ti++) {
		uartOut.printf("%d\r\n", t[ti]);
		while(uartOut.USARTdrv->GetStatus().tx_busy);
	}
	*/
}

// Reads specified number of bits from the AVCLan.
// nbBits (byte) -> Number of bits to read.
// Return (word) -> Data value read.
word AVCLanDrv::readBits (byte nbBits){
	word data  = 0;
	_parityBit = 0;
	
	while (nbBits-- > 0){
		// Insert new bit
		data <<= 1;
		// Wait until rising edge of new bit.
		while (inputIsClear());

		// Reset timer to measure bit length.
		timer.reset();
		// Wait until falling edge.
		while (inputIsSet());
		// Compare half way between a '1' (20 us) and a '0' (32 us ): 32 - (32 - 20) /2 = 26 us
		if (timer.getTicks() < AVC_BIT_0_HOLD_ON_MIN_LENGTH){
			// Set new bit.
			data |= 0x0001;
			// Adjust parity.
			_parityBit = ! _parityBit;

			uartOut.send("1",1);
		} else {
			uartOut.send("0",1);
		}
	}
	uartOut.send(" ",1);
	while (inputIsClear() && timer.getTicks() < AVC_NORMAL_BIT_LENGTH);
	return data;
}

// Read incoming messages on the AVCLan.
// Return true if success.
byte AVCLanDrv::_readMessage (){
	// Start bit.
	while (inputIsClear());
	timer.reset();
	// Wait until falling edge.
	while (inputIsSet()){
		if (timer.getTicks() > AVC_TIMEOUT_LENGTH) {
			//TCCR2B=0x02;      // prescaler 8

			return 1;
		}
	}
	uint32_t t = timer.getTicks();
	if (t < AVC_START_BIT_HOLD_ON_MIN_LENGTH){
        bSerial.print("Error t < AVC_START_BIT_HOLD_ON_MIN_LENGTH: ");
        bSerial.printHex8(timer.getTicks());

		return 2;
	}

	uint32_t d1 = readBits(32);
	uint32_t d2 = readBits(32);

	uartOut.printf("%08d %08d\r\n", d1, d2);
	/*
	broadcast = readBits(1);

	masterAddress = readBits(12);
	bool p = _parityBit;
	if (p != readBits(1)){

		return 3;
	}
  
	slaveAddress = readBits(12);       
	p = _parityBit;
	if (p != readBits(1)){

		return 4;
	}

	bool forMe = ( slaveAddress == deviceAddress );

	if (forMe){
		// Send ACK.
		avcOutEnable();
		send1BitWord(0);
		avcOutDisable();
	}else{  
		readBits(1);
	}

	// Control 
	readBits(4);
	p = _parityBit;
	if (p != readBits(1)){

		return 5;
	}

	if (forMe){
		// Send ACK.
		avcOutEnable();
		send1BitWord(0);
		avcOutDisable();
	}else{  
		readBits(1);
	}

	dataSize = readBits(8);
	p = _parityBit;
	if (p != readBits(1)){

		return 6;
	}

	if (forMe){
		// Send ACK.
		avcOutEnable();
		send1BitWord(0);
		avcOutDisable();
	}else{  
		readBits(1);
	}
	if (dataSize > AVC_MAXMSGLEN){

		return 7;
	}
	byte i;
	for (i = 0; i < dataSize; i++ ){
		message[i] = readBits(8);
		p = _parityBit;
		if (p != readBits(1)){

			return 8;
		}

		if (forMe){
			// Send ACK.
			avcOutEnable();
			send1BitWord(0);
			avcOutDisable();
		}else{  
			readBits(1);
		}
	}
*/
	return 0;   
}

// Read incoming messages on the AVCLan, log message through serial port
// Return true if success.
byte AVCLanDrv::readMessage (){
	byte res = _readMessage();
	if (!res){
		//printMessage(true);
	}else{
		bSerial.print("R");
		bSerial.printHex4(res);
		bSerial.println();
		while (!isAvcBusFree());
	}
	return res;
}

// Send a start bit to the AVCLan
void AVCLanDrv::sendStartBit (){
	// Reset timer to measure bit length.
	//TCCR2B=0x03;      // prescaler 32
	timer.reset();
	outputSet1();

	// Pulse level high duration.
	while ( timer.getTicks() < AVC_START_BIT_HOLD_ON_LENGTH );
	outputSet0();

	// Pulse level low duration until ~185 us.
	while ( timer.getTicks() < AVC_START_BIT_LENGTH );
	//TCCR2B=0x02;      // prescaler 8

}

// Send a 1 bit word to the AVCLan
void AVCLanDrv::send1BitWord (bool data){
	// Reset timer to measure bit length.
	timer.reset();
	outputSet1();

	if (data){
		while (timer.getTicks() < AVC_BIT_1_HOLD_ON_LENGTH);
	}else{
		while (timer.getTicks() < AVC_BIT_0_HOLD_ON_LENGTH);
	}

	outputSet0();
	while (timer.getTicks() <  AVC_NORMAL_BIT_LENGTH);
}

// Send a 4 bit word to the AVCLan
void AVCLanDrv::send4BitWord (byte data){
	_parityBit = 0;

	// Most significant bit out first.   
	for ( char nbBits = 0; nbBits < 4; nbBits++ ){
		// Reset timer to measure bit length.
		timer.reset();
		outputSet1();

		if (data & 0x8){
			// Adjust parity.
			_parityBit = ! _parityBit;
			while ( timer.getTicks() < AVC_BIT_1_HOLD_ON_LENGTH );
		}else{
			while ( timer.getTicks() < AVC_BIT_0_HOLD_ON_LENGTH );
		}

		outputSet0();
		// Hold output low until end of bit.
		while ( timer.getTicks() < AVC_NORMAL_BIT_LENGTH );

		// Fetch next bit.
		data <<= 1;
	}
}

// Send a 8 bit word to the AVCLan
void AVCLanDrv::send8BitWord (byte data){
	_parityBit = 0;

	// Most significant bit out first.   
	for ( char nbBits = 0; nbBits < 8; nbBits++ ){
		// Reset timer to measure bit length.
		timer.reset();
		outputSet1();

		if (data & 0x80){
			// Adjust parity.
			_parityBit = ! _parityBit;
			while ( timer.getTicks() < AVC_BIT_1_HOLD_ON_LENGTH );
		}else{
			while ( timer.getTicks() < AVC_BIT_0_HOLD_ON_LENGTH );
		}

		outputSet0();
		// Hold output low until end of bit.
		while ( timer.getTicks() < AVC_NORMAL_BIT_LENGTH );

		// Fetch next bit.
		data <<= 1;
	}
}

// Send a 12 bit word to the AVCLan
void AVCLanDrv::send12BitWord (word data){
	_parityBit = 0;

	// Most significant bit out first.   
	for ( char nbBits = 0; nbBits < 12; nbBits++ ){
		// Reset timer to measure bit length.
		timer.reset();
		outputSet1();

		if (data & 0x0800){
			// Adjust parity.
			_parityBit = ! _parityBit;
			while ( timer.getTicks() < AVC_BIT_1_HOLD_ON_LENGTH );
		}else{
			while ( timer.getTicks() < AVC_BIT_0_HOLD_ON_LENGTH );
		}

		outputSet0();
		// Hold output low until end of bit.
		while ( timer.getTicks() < AVC_NORMAL_BIT_LENGTH );

		// Fetch next bit.
		data <<= 1;
	}
}

//  determine whether the bus is free (no tx/rx).
//  return TRUE is bus is free.
bool AVCLanDrv::isAvcBusFree (void){
	// Reset timer.
	timer.reset();

	while (inputIsClear()){
		// We assume the bus is free if anything happens for the length of 1 bit.
		if (timer.getTicks() > AVC_NORMAL_BIT_LENGTH){
			return true;
		}
	}

	return false;
}

// reads the acknowledge bit the AVCLan
// return TRUE if ack detected else FALSE.
bool AVCLanDrv::readAcknowledge (void){
	// The acknowledge pattern is very tricky: the sender shall drive the bus for the equivalent
	// of a bit '1' (20 us) then release the bus and listen. At this point the target shall have
	// taken over the bus maintaining the pulse until the equivalent of a bit '0' (32 us) is formed.

	// Reset timer to measure bit length.
	timer.reset();
	outputSet1();

	// Generate bit '0'.
	while (timer.getTicks() < AVC_BIT_1_HOLD_ON_LENGTH);
	outputSet0();
	
	avcOutDisable();
	
	while (timer.getTicks() < AVC_BIT_1_HOLD_ON_LENGTH + AVC_1U_LENGTH);
	// Measure final resulting bit.
	while ( inputIsSet() );

	// Sample half-way through bit '0' (26 us) to detect whether the target is acknowledging.
	if (timer.getTicks() > AVC_BIT_0_HOLD_ON_MIN_LENGTH){
		// Slave is acknowledging (ack = 0). Wait until end of ack bit.
		while (inputIsSet() );
		avcOutEnable();
		return true;
	}

	// No sign of life on the bus.
	return false;    
}

// sends ack bit if I am broadcasting otherwise wait and return received ack bit.
// return FALSE if ack bit not detected.
bool AVCLanDrv::handleAcknowledge (void){
	if (broadcast == AVC_MSG_BROADCAST){   
		// Acknowledge.    
		send1BitWord(0);
		return true;
	}

	// TODO
	if (broadcast == AVC_MSG_DIRECT){ 
		// Acknowledge. 
		send1BitWord(1);
		return true;
	 }
	// Return acknowledge bit.
	return readAcknowledge();
} 

// sends the message in global registers on the AVC LAN bus.
// return 0 if successful else error code
byte AVCLanDrv::_sendMessage (void){  
	while (!isAvcBusFree());
	
	avcOutEnable();
	
	// Send start bit.
	sendStartBit();

	// Broadcast bit.
	send1BitWord(broadcast);

	// Master address = me.
	send12BitWord(masterAddress);
	send1BitWord(_parityBit);

	// Slave address = head unit (HU).
	send12BitWord(slaveAddress);
	send1BitWord(_parityBit);
	if (!handleAcknowledge()){
		avcOutDisable();

		return 1;
	}

	// Control flag + parity.
	send4BitWord(AVC_CONTROL_FLAGS);
	send1BitWord(_parityBit);
	if (!handleAcknowledge()){
		avcOutDisable();

		return 2;
	}

	// Data length + parity.
	send8BitWord(dataSize);
	send1BitWord(_parityBit);
	if (!handleAcknowledge()){
		avcOutDisable();

		return 3;
	}

	for (byte i = 0; i < dataSize; i++){
		send8BitWord(message[i]);
		send1BitWord(_parityBit);
		if (!handleAcknowledge()){
			avcOutDisable();

			return false;
		}
	}
	avcOutDisable();

	return 0;
}

// sends the message in global registers on the AVC LAN bus, log message through serial port
// return 0 if successful else error code
byte AVCLanDrv::sendMessage (void){
	byte sc = AVC_MAXSENDATTEMPTS;
	byte res;
	do{
		res = _sendMessage();
		if (!res){
			printMessage(false);
		}else{
			bSerial.print("W");
			bSerial.printHex4(res);
			bSerial.println();
			while (!isAvcBusFree());
		}
		sc--;
	}while (sc && res);
	return res;
}

// sends the message for given mesage ID on the AVC LAN bus, log message through serial port
// return 0 if successful else error code
byte AVCLanDrv::sendMessage (AvcOutMessage *msg){
	loadMessage(msg);
	return sendMessage();
}
// print message to serial port
void AVCLanDrv::printMessage(bool incoming){
	if (incoming){
		bSerial.print("< ");
	}else{
		bSerial.print("> ");
	}  
	if (broadcast == AVC_MSG_BROADCAST){
		bSerial.print("b ");
	}else{ 
		bSerial.print("d ");
	}
	bSerial.printHex4(masterAddress >> 8);
	bSerial.printHex8(masterAddress);
	bSerial.print("->");

	bSerial.printHex4(slaveAddress >> 8);
	bSerial.printHex8(slaveAddress);
	bSerial.print(" ");
	bSerial.printHex8(dataSize);
    if (slaveAddress == 0x0FFF) {
        bSerial.print("          ");
    } else {
        bSerial.print(" ");
    }

	for (byte i = 0; i < dataSize; i++) {
		bSerial.printHex8(message[i]);
        if (i == 0) {
            if (slaveAddress == 0x01FF) { // separate logical src and dst addrs
                bSerial.print("       ");
            } else if (slaveAddress == 0x0FFF) {
            
            } else {
                bSerial.print("->");
            }
        } else if (i == 1) {
            if (slaveAddress == 0x01FF) {
            	bSerial.print("    ");
            } else if (slaveAddress == 0x0FFF) {
                
            } else {
                bSerial.print("   ");
            }
        }
	}
	bSerial.println(); 

}

// Use the last received message to determine the corresponding action ID
byte AVCLanDrv::getActionID(AvcInMessageTable messageTable[], byte mtSize){
	/*
	if (slaveAddress != deviceAddress && slaveAddress != 0x0FFF) return ACT_NONE;
	for (byte msg = 0; msg < mtSize; msg++){
		bool found = true;
		
		if (dataSize != pgm_read_byte_near(&messageTable[msg].dataSize)){
			continue;
		}
		for (byte i = 0; i < dataSize; i++){
			if (message[i] != pgm_read_byte_near(&messageTable[msg].data[i])){
				found = false;
				break;
			}
		}

		if (found){
			return pgm_read_byte_near(&messageTable[msg].actionID);
		}
	}
	*/
	return ACT_NONE;
}

// Use the last received message to determine the corresponding action ID, use masked message table
byte AVCLanDrv::getActionID(AvcInMaskedMessageTable messageTable[], byte mtSize){
	/*
	if (slaveAddress != deviceAddress && slaveAddress != 0x0FFF) return ACT_NONE;
	for (byte msg = 0; msg < mtSize; msg++){
		bool found = true;
		
		if (dataSize != pgm_read_byte_near(&messageTable[msg].dataSize)){
			continue;
		}
		word mask = pgm_read_byte_near(&messageTable[msg].mask);
		for (byte i = 0; i < dataSize; i++){
			if (mask & _BV(i)) continue;
			if (message[i] != pgm_read_byte_near(&messageTable[msg].data[i])){
				found = false;
				break;
			}
		}

		if (found){
			return pgm_read_byte_near(&messageTable[msg].actionID);
		}
	}
	*/
	return ACT_NONE;
}

// Loads message data for given mesage ID.
void AVCLanDrv::loadMessage(AvcOutMessage *msg){
	/*
	broadcast = pgm_read_byte_near(&msg->broadcast);
	masterAddress = deviceAddress;

	if (broadcast == AVC_MSG_BROADCAST)
		slaveAddress = 0x01FF;
	else
		slaveAddress = headAddress;

	dataSize = pgm_read_byte_near( &msg->dataSize ); 

	for (byte i = 0; i < dataSize; i++ ){
		message[i] = pgm_read_byte_near( &msg->data[i] );
	}
	*/
};
