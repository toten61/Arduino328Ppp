#include "TimerInterrupts.h"
    
#include <stdlib.h> 

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//add one level of indirection that should allow to overwrite these ISRs from user-code

//static inline void Timer0_OVF_ISR() {} //default version

static inline void Timer0_COMPA_ISR() {} //default version

static inline void Timer0_COMPB_ISR() {} //default version

static inline void Timer1_OVF_ISR() {} //default version

static inline void Timer1_COMPA_ISR() {} //default version

static inline void Timer1_COMPB_ISR() {} //default version

static inline void Timer2_OVF_ISR() {} //default version

static inline void Timer2_COMPA_ISR() {} //default version

static inline void Timer2_COMPB_ISR() {} //default version


/*ISR(TIMER0_OVF_vect) {
    Timer0_OVF_ISR();
    //Test();
}*/

ISR(TIMER0_COMPA_vect) {
    Timer0_COMPA_ISR();
}
ISR(TIMER0_COMPB_vect) {
    Timer0_COMPB_ISR();
}

ISR(TIMER1_OVF_vect) {
    Timer1_OVF_ISR();
}
ISR(TIMER1_COMPA_vect) {
    Timer1_COMPA_ISR();
}
ISR(TIMER1_COMPB_vect) {
    Timer1_COMPB_ISR();
}

ISR(TIMER2_OVF_vect) {
    Timer2_OVF_ISR();
}
ISR(TIMER2_COMPA_vect) {
    Timer2_COMPA_ISR();
}
ISR(TIMER2_COMPB_vect) {
    Timer2_COMPB_ISR();
}