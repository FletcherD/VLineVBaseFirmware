/*
  BuffSerial.h - serial with transmit buffer library for Wiring
  Created by Kochetkov Aleksey, 28.11.2009
  Version 0.1.2
*/

#ifndef BuffSerial_h
#define BuffSerial_h

class BuffSerial{
	public:
		void		begin(long);
		void 		sendByte(uint8_t);
		void		print(const char*);
		void		print(const char);
		void		print_p(const char*);
		void		println(const char*);
		void		println(const char);
		void		println(void);
		void		println_p(const char*);
		void		printHex4(uint8_t);
		void		printHex8(uint8_t);
		void		printDec(uint8_t);
		bool		rxEnabled(void);
		uint8_t		rxRead(void);	
};

extern BuffSerial bSerial;

#endif

