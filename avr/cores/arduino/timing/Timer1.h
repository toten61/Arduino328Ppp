#pragma once

#include <arduino.h>

#include "./TemplateTimer.h"

namespace Atmega328Ppp {

    namespace Timer {
        /*
        * According to Atmega 328P datasheet page 109 these modes have the following uses:
        * 
        * Mode | Operation Mode           |   TOP   | Update of OCR1x at | TOV1 Flag Set on
        *  0   |     Normal               | 0xFFFF  | Immediate          | MAX
        *  1   | Phase-Correct PWM, 8 Bit | 0x00FF  | TOP                | BOTTOM
        *  2   | Phase-Correct PWM, 9 Bit | 0x01FF  | TOP                | BOTTOM
        *  3   | Phase-Correct PWM,10 Bit | 0x03FF  | TOP                | BOTTOM
        *  4   | CTC                      | OCR1A   | Immediate          | MAX
        *  5   | Fast PWM, 8 Bit          | 0x00FF  | BOTTOM             | TOP
        *  6   | Fast PWM, 9 Bit          | 0x01FF  | BOTTOM             | TOP
        *  7   | Fast PWM, 10 Bit         | 0x03FF  | BOTTOM             | TOP
        *  8   | Phase & Freq. Correct PWM| ICR1    | BOTTOM             | BOTTOM
        *  9   | Phase & Freq. Correct PWM| OCR1A   | BOTTOM             | BOTTOM
        * 10   | Phase-Correct PWM        | ICR1    | TOP                | BOTTOM
        * 11   | Phase-Correct PWM        | OCR1A   | TOP                | BOTTOM
        * 12   | CTC                      | ICR1    | Immediate          | MAX
        * 13   | Reserved                 | -       | -                  | -
        * 14   | Fast PWM                 | ICR1    | BOTTOM             | TOP
        * 15   | Fast PWM                 | OCR1A   | BOTTOM             | TOP
        */

        class WaveformMode1 : public WaveformMode<uint16_t> {

        public: 
            static const WaveformMode1 
                Normal_Fixed,
                PWM_PhaseCorrect_Fixed_8,
                PWM_PhaseCorrect_Fixed_9,
                PWM_PhaseCorrect_Fixed_10,
                CTC_OCR1A,
                PWM_Fast_Fixed_8,
                PWM_Fast_Fixed_9,
                PWM_Fast_Fixed_10,
                PWM_PhaseFreqCorrect_ICR1,
                PWM_PhaseFreqCorrect_OCR1A,
                PWM_PhaseCorrect_ICR1,
                PWM_PhaseCorrect_OCR1A,
                CTC_ICR1,
                PWM_Fast_ICR1,
                PWM_Fast_OCR1A;
          
          inline uint8_t TimerControlRegisterA() const;
          inline uint8_t TimerControlRegisterB() const;
        private:
            WaveformMode1(const uint8_t wgm, const uint16_t topv);
            WaveformMode1(const uint8_t wgm, volatile uint16_t *const topp);
        };

        enum class Prescaler1 {
            None = 0x00,
            Div1 = 0x01,
            Div8 = 0x02,
            Div64 = 0x03,
            Div256 = 0x04,
            Div1024 = 0x05,
            ExtFallingEdgeT1 = 0x06,
            ExtRisingEdgeT1 = 0x07
        };



        class Timer1 : public TemplateTimer<uint16_t, WaveformMode1, Prescaler1, 2> {
        
            /*
            * 
            * Operation Modes:
            * Normal: Incrementing counter with 16-bit, overflow, timer overflow flag TOV1 set when timer becomes 0, can be cleared by interrupt, output compare units can be used to generate interrupts
            * CTC: The counter register is cleared when it reaches the value of the compare register (either ICR1 or OCR1A), can be used to generate waveform output, NOT DOUBLE BUFFERED, max frequency: F_CPU / 2
            *  f_OCnA = F_CPU / (2 * * Prescaler * (1 + OCRnA))
            * Fast PWM: Count from BOTTOM to TOP (timer registers) and restarts from B. In non-inverting output mode pin OC1x is cleared on compare match between TCNT1 and OCR1x and set at BOTTOM, in inverting mode the operation is inverted
            */
            
            /*
            * According to Atmega 328P datasheet page 108f the toggle setting has different effects based on the waveform mode used:
            * Waveform Mode 9/11/14/15: Toggle OC1A on compare match, OC1B disconnected
            * Additionally for phase correct and for phase & frequency correct PWM the Clear and set mode may be inverted depending on the counting direction
            */
        public:  
            Timer1() = delete;         
            Timer1(
                const WaveformMode1& waveformMode, 
                const Prescaler1& prescaler);
            virtual void activate(const uint16_t& timerTopValue, bool enOvflInt = false) override;
            virtual void activateCompareChannel(const Channel& channel, const uint16_t& compareValue, const CompareOutputMode& outputMode = CompareOutputMode::Disconnected, const bool enableInterrupt = false) override;
            virtual void activateOutputPin(const Channel& channel) override;
            /*
            * This only has an effect in non-PWM modes 
            */
            
            //void forceOutputCompareForChannel(const Channel1& m_channel);
        };
    }
}
