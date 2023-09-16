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

	AVCLanMessage(IEBusMessage m);

	void setOperands(const std::vector<DataValue>& oIn);

	std::vector<DataValue> getOperands() const;
};

#endif /* AVCLANMESSAGE_H_ */
