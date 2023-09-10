/*
 * AVCLanMessage.h
 *
 *  Created on: Sep 8, 2023
 *      Author: fletcher
 */

#ifndef AVCLANMESSAGE_H_
#define AVCLANMESSAGE_H_

#include "IEBusMessage.h"
#include <vector>

typedef uint8_t Function;
typedef uint8_t Opcode;

class AVCLanMessage : public IEBusMessage {
public:
	size_t dIndex;
	Function& srcFunction;
	Function& dstFunction;
	Opcode& opcode;
	DataValue* operands;
	size_t nOperands;

	AVCLanMessage(IEBusMessage m)
	: IEBusMessage(m),
	  dIndex(broadcast==BROADCAST ? 0 : 1),
	  nOperands(dataLength - dIndex - 3),
	  srcFunction(data[dIndex++]),
	  dstFunction(data[dIndex++]),
	  opcode(data[dIndex++]),
	  operands(data+dIndex)
	  {}

	void setOperands(std::vector<DataValue> oIn)
	{
		std::copy(oIn.cbegin(), oIn.cend(), operands);
	}
};

#endif /* AVCLANMESSAGE_H_ */
