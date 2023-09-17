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
	AVCLanMessage(BroadcastValue broadcast,
				  Address masterAddress,
				  Address slaveAddress,
				  Function srcFunction,
				  Function dstFunction,
				  Opcode opcode,
				  const std::vector<DataValue>& operands
				  );

	void setOperands(const std::vector<DataValue>& oIn);

	std::vector<DataValue> getOperands() const;
};

class AVCLanMessageListener {

};

#endif /* AVCLANMESSAGE_H_ */
