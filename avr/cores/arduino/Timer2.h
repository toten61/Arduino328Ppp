#pragma once

#include <arduino.h>

#include <TemplateTimer.h>

namespace Atmega328Ppp {

    namespace Timer {
        /*
        * Structure encapsulated the different waveform operation modes of the Timer 2
        * According to Atmega 328P datasheet page 130 these modes have the following properties:
        * 
        * Mode | Operation Mode   |  TOP   | Update OCR2x at | TOV2 Flag Set on
        *  0   | Normal           |  0xFF  | Immediate       | MAX
        *  1   | Phase Correct PWM|  0xFF  | TOP             | BOTTOM
        *  2   | CTC              |  OCR2A | Immediate       | MAX
        *  3   | Fast PWM         |  0xFF  | BOTTOM          | MAX
        *  4   | Reserved         |   -    |   -             |  - 
        *  5   | Phase Correct PWM| OCR2A  | TOP             | BOTTOM
        *  6   | Reserved         |   -    | -               |  -
        *  7   | Fast PWM         | OCR2A  | BOTTOM          | TOP
        */
        class WaveformMode2 : public WaveformMode<uint8_t> {

        public: 
            static const WaveformMode2 
                Normal_Fixed,
                PWM_PhaseCorrect_Fixed,
                CTC_OCR2A,
                PWM_Fast_Fixed,
                PWM_PhaseCorrect_OCR2A,
                PWM_Fast_OCR2A;
          
          inline uint8_t TimerControlRegisterA() const;
          inline uint8_t TimerControlRegisterB() const;
        private:
            WaveformMode2(const uint8_t wgm, const uint8_t topv);
            WaveformMode2(const uint8_t wgm, volatile uint8_t *const topp);
        };

        enum class Prescaler2 {
            None = 0x00,
            Div1 = 0x01,
            Div8 = 0x02,
            Div32 = 0x03,
            Div64 = 0x04,
            Div128 = 0x05,
            Div256 = 0x06,
            Div1024 = 0x07
        };


        class Timer2 : public TemplateTimer<uint8_t, WaveformMode2, Prescaler2, 2> {
        
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
            Timer2() = delete;
            Timer2(
                const WaveformMode2& waveformMode,
                const Prescaler2& p);

            //set register values
            virtual void activate(const uint8_t& ttop,
                const bool enableInterrupt = false) override;
            virtual void activateCompareChannel(
                const Channel& channel, 
                const uint8_t& compareValue, 
                const CompareOutputMode& outputMode = CompareOutputMode::Disconnected, 
                const bool enableInterrupt = false) override;
            virtual void activateOutputPin(const Channel& channel) override;
            
            /*
            * This only has an effect in non-PWM modes 
            */
            
            
            //void forceOutputCompareForChannel(const Channel2& m_channel);
        };
    }
}
