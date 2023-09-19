/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef VCORE_POWER_H_
#define VCORE_POWER_H_

#include "cmsis_device.h"
#include "util.h"

class power
{
public:
	static constexpr PinCfgType VCore5V = {
		.PortNum =  	1,
		.PinNum = 		15,
		.ActiveLow	=	true
	};
	static constexpr PinCfgType VCore3P3V = {
		.PortNum =  	1,
		.PinNum = 		29,
		.ActiveLow	=	true
	};
	static constexpr PinCfgType VBase5V = {
		.PortNum =  	1,
		.PinNum = 		19,
		.ActiveLow	=	true
	};
	static constexpr PinCfgType Usb5V = {
		.PortNum =  	1,
		.PinNum = 		27,
		.ActiveLow	=	true
	};
	static constexpr PinCfgType AudioAmp = {
		.PortNum =  	1,
		.PinNum = 		14,
		.ActiveLow	=	true
	};
	static constexpr PinCfgType VideoSel = {
		.PortNum =  	1,
		.PinNum = 		1,
	};
	static constexpr PinCfgType VideoReset = {
		.PortNum =  	1,
		.PinNum = 		4,
	};

	power();

	static void
	turnOn(PinCfgType powerPin);

	static void
	turnOff(PinCfgType powerPin);
};

// ----------------------------------------------------------------------------

#endif
