/*
  BuffSerial.cpp - serial with transmit buffer library for Wiring
  Created by Kochetkov Aleksey, 28.11.2009
  Version 0.1.2
*/
#include <stdio.h>
#include "diag/trace.h"

#include "BuffSerial.h"
#include "uart.h"

// serial init
void BuffSerial::begin(long speed){
}

// print string
void BuffSerial::print(const char *pBuf){
	uartOut.printf(pBuf);
}

void BuffSerial::println(const char *pBuf){
	print(pBuf);
	println();
}

void BuffSerial::println(void){
	print("\r\n");
}

void BuffSerial::printHex4(uint8_t data){
	uartOut.printf("0x%04x\n", data);
}

void BuffSerial::printHex8(uint8_t data){
	uartOut.printf("0x%08x\n", data);
} 

void BuffSerial::printDec(uint8_t data){
	uartOut.printf("%d\n", data);
}

// check rx buffer not empty
bool BuffSerial::rxEnabled(void){  
	//return rxEnd;
	return false;
}

uint8_t BuffSerial::rxRead(void){
	return 0;
}

BuffSerial bSerial;
