#pragma once

/*
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
    Note (toten61): This file was created by me, but the content
    was initially written and published by the Arduino team
*/

#include "./Timer0.h"

namespace Arduino::Timing {

    void init();

    unsigned long millis(void);
    unsigned long micros(void);
    void delay(unsigned long ms);
    void delayMicroseconds(unsigned int us);

}