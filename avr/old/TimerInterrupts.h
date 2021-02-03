#ifndef TIMER_INTERTUPTS_H
    #define TIMER_INTERTUPTS_H

    
    #include <stdlib.h> 

    #include <avr/pgmspace.h>
    #include <avr/io.h>
    #include <avr/interrupt.h>

    //add one level of indirection that should allow to overwrite these ISRs from user-code
    
    //inline void Timer0_OVF_ISR() __attribute__((weak));

    inline void Timer0_COMPA_ISR() __attribute__((weak));

    inline void Timer0_COMPB_ISR() __attribute__((weak));

    inline void Timer1_OVF_ISR() __attribute__((weak));
    
    inline void Timer1_COMPA_ISR() __attribute__((weak));
    
    inline void Timer1_COMPB_ISR() __attribute__((weak));
    
    inline void Timer2_OVF_ISR() __attribute__((weak));
    
    inline void Timer2_COMPA_ISR() __attribute__((weak));

    inline void Timer2_COMPB_ISR() __attribute__((weak));

#endif // TIMER_INTERTUPTS_H