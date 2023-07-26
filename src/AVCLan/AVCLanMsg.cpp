/*
 * AVCLanMsg.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: fletcher
 */
#include "AVCLanMsg.h"
#include "util.h"

constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::Broadcast;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::MasterAddress;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::SlaveAddress;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::Control;
constexpr AVCLanMsg::AVCLanMsgField AVCLanMsg::DataLength;

AVCLanMsg::AVCLanMsg(uint8_t* messageBuf)
	: messageBuf(messageBuf)
{
}


