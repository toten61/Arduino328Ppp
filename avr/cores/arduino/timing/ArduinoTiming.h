#pragma once


#include "./Timer0.h"

namespace Arduino::Timing {

    void init();

    unsigned long millis(void);
    unsigned long micros(void);
    void delay(unsigned long ms);
    void delayMicroseconds(unsigned int us);

}