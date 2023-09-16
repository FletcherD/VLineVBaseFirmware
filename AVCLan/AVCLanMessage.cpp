/*
 * AVCLanMessage.cpp
 *
 *  Created on: Sep 8, 2023
 *      Author: fletcher
 */
#include "AVCLanMessage.h"

AVCLanMessage::AVCLanMessage(IEBusMessage m)
: IEBusMessage(m),
dIndex(broadcast==BROADCAST ? 0 : 1),
srcFunction(data[dIndex++]),
dstFunction(data[dIndex++]),
opcode(data[dIndex++]),
operands(data+dIndex),
nOperands(dataLength - 3 - (broadcast==BROADCAST ? 0 : 1))
{}

void
AVCLanMessage::setOperands(const std::vector<DataValue>& oIn)
{
	dataLength = (broadcast==BROADCAST ? 0 : 1) + 3 + oIn.size();
	std::copy(oIn.cbegin(), oIn.cend(), operands);
	nOperands = oIn.size();
}

std::vector<DataValue>
AVCLanMessage::getOperands() const
{
	return std::vector<DataValue>{operands, operands+nOperands};
}