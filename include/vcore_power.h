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

#define POWER_PORT_NUMBER              (1)
#define POWER_USB5V_PIN                (27)
#define POWER_VCORE_5V_PIN             (15)
#define POWER_VCORE_3P3V_PIN           (29)

// ----------------------------------------------------------------------------

static constexpr int PowerPinList[3] = {
	 POWER_USB5V_PIN,
	 POWER_VCORE_5V_PIN,
	 POWER_VCORE_3P3V_PIN
};

class power
{
public:
  enum PowerNode {
	  Usb5v = 0,
	  Vcore_5v = 1,
	  Vcore_3p3v = 2
  };

  power();

  void
  turn_on(PowerNode powerNode);

  void
  turn_off(PowerNode powerNode);
};

// ----------------------------------------------------------------------------

#endif
