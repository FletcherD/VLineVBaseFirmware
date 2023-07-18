/*
  AVCLanDrv.h - AVCLan Library for 'duino / Wiring
  Created by Kochetkov Aleksey, 04.08.2010
  Version 0.3.1
*/

#ifndef AVCLanDrv_h
#define AVCLanDrv_h

#include "cmsis_device.h"
extern "C" {
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
}
#include "timer.h"
#include "uart.h"

#define AVCLANDRV_VERSION "0.3.1"

typedef uint8_t 	byte;
typedef uint32_t 	word;

typedef enum
{   // No this is not a mistake, broadcast = 0!
    AVC_MSG_DIRECT    = 1,
    AVC_MSG_BROADCAST = 0
} AvcTransmissionMode;

#define ACT_NONE 0  // no action
#define EV_NONE	 0	// no event

typedef struct
{
	byte	actionID;           // Action id
	byte	dataSize;           // message size (bytes)
	byte	data[12];           // message
} AvcInMessageTable;

typedef struct
{
	byte	actionID;           // Action id
	byte	dataSize;           // message size (bytes)
	byte	data[14];           // message
	word	mask;				// mask, set bit = 1 in not checked position (1<<5 or _BV(5) - datap[5] not checked)
} AvcInMaskedMessageTable;

typedef struct
{
	AvcTransmissionMode broadcast;          // Transmission mode: normal (1) or broadcast (0).
    byte                dataSize;           // message size (bytes)
    byte                data[14];           // message
} AvcOutMessage;


class AVCLanDrv{
	private:
		p_timer timer;
		uint32_t uS_to_ticks(uint32_t uS) {
			return 96 * uS * (timer.tickRate / 1000000);
		}


		bool inputIsSet()			{ return !GPIO_PinRead(AVC_RX_PIN.port, AVC_RX_PIN.num); }
		bool inputIsClear()			{ return !inputIsSet(); }
		void setStandby(bool isOn)	{ GPIO_PinWrite(AVC_STB_PIN.port, AVC_STB_PIN.num, isOn); }
		void setEnabled(bool isOn)	{ GPIO_PinWrite(AVC_EN_PIN.port, AVC_EN_PIN.num, isOn); }
		// TX pin on this transceiver is active low
		void setTx(bool isOn)		{ GPIO_PinWrite(AVC_TX_PIN.port, AVC_TX_PIN.num, !isOn); }

		void avcOutEnable()		{};
		void avcOutDisable()	{};
		void outputSet1()		{};
		void outputSet0()		{};

		const uint32_t AVC_NORMAL_BIT_LENGTH				= uS_to_ticks(39);
		const uint32_t AVC_BIT_1_HOLD_ON_LENGTH				= uS_to_ticks(20);
		const uint32_t AVC_BIT_0_HOLD_ON_LENGTH				= uS_to_ticks(32);
		const uint32_t AVC_BIT_0_HOLD_ON_MIN_LENGTH			= uS_to_ticks(26);
		const uint32_t AVC_START_BIT_LENGTH					= uS_to_ticks(186);
		const uint32_t AVC_START_BIT_HOLD_ON_LENGTH			= uS_to_ticks(168);
		const uint32_t AVC_START_BIT_HOLD_ON_MIN_LENGTH		= uS_to_ticks(44);
		const uint32_t AVC_1U_LENGTH						= uS_to_ticks(1);
		const uint32_t AVC_TIMEOUT_LENGTH					= uS_to_ticks(1000);

		static constexpr uint32_t AVC_MAXSENDATTEMPTS = 32;
		static constexpr uint32_t AVC_MAXMSGLEN = 32;
		static constexpr uint32_t AVC_CONTROL_FLAGS = 0xF;

	public:
		bool	broadcast;
		word	masterAddress;
		word	slaveAddress;
		word	deviceAddress;
		word	headAddress;
		byte	dataSize;
		byte	message[AVC_MAXMSGLEN];
		byte	event;
		byte	actionID;
		bool	readonly;
        bool    log;
		void	begin ();
		byte	readMessage (void);
		byte	sendMessage (void);
		byte	sendMessage (AvcOutMessage*);
		void 	printMessage (bool incoming);
		bool	isAvcBusFree (void);
		byte	getActionID (AvcInMessageTable messageTable[], byte mtSize);
		byte	getActionID (AvcInMaskedMessageTable messageTable[], byte mtSize);
		void	loadMessage (AvcOutMessage*);
	private:
		bool	_parityBit;
		word	readBits (byte nbBits);
		byte	_readMessage (void);
		byte	_sendMessage (void);
		void	sendStartBit (void);
		void	send1BitWord (bool data);
		void	send4BitWord (byte data);
		void	send8BitWord (byte data);
		void	send12BitWord (word data);
		bool	readAcknowledge (void);
		bool	handleAcknowledge (void);
};

#endif
