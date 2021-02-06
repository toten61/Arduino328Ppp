#pragma once

/*
  Copyright (c) 2021 toten61.  All right reserved.

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
    This file is part of my (toten61) fork/modification of the official Arduino SDK
    and aims at making the functionality and configuration of Timer0
    easier and more comfortable to use. It uses the same license 
    as the Official Arduino SDK: LGPL 2.1 or later
*/

#include <arduino.h>

#include "./TemplateTimer.h"

namespace Atmega328Ppp {

    namespace Timer {
        /*
        * Structure encapsulated the different waveform operation modes of the Timer 2
        * According to Atmega 328P datasheet page 130 these modes have the following properties:
        * 
        * Mode | Operation Mode   |  TOP   | Update OCR0x at | TOV0 Flag Set on
        *  0   | Normal           |  0xFF  | Immediate       | MAX
        *  1   | Phase Correct PWM|  0xFF  | TOP             | BOTTOM
        *  2   | CTC              |  OCR0A | Immediate       | MAX
        *  3   | Fast PWM         |  0xFF  | BOTTOM          | MAX
        *  4   | Reserved         |   -    |   -             |  - 
        *  5   | Phase Correct PWM| OCR0A  | TOP             | BOTTOM
        *  6   | Reserved         |   -    | -               |  -
        *  7   | Fast PWM         | OCR0A  | BOTTOM          | TOP
        */
        class WaveformMode0 : public WaveformMode<uint8_t> {

        public: 
            static const WaveformMode0 
                Normal_Fixed,
                PWM_PhaseCorrect_Fixed,
                CTC_OCR0A,
                PWM_Fast_Fixed,
                PWM_PhaseCorrect_OCR0A,
                PWM_Fast_OCR0A;
          
          inline uint8_t TimerControlRegisterA() const;
          inline uint8_t TimerControlRegisterB() const;
        private:
            WaveformMode0(const uint8_t wgm, const uint8_t topv);
            WaveformMode0(const uint8_t wgm, volatile uint8_t *const topp);
        };

        enum class Prescaler0 {
            None = 0x00,
            Div1 = 0x01,
            Div8 = 0x02,
            Div64 = 0x03,
            Div256 = 0x04,
            Div1024 = 0x05,
            ExtFallingEdgeT0 = 0x06,
            ExtRisingEdgeT0 = 0x07
        };


        class Timer0 : public TemplateTimer<uint8_t, WaveformMode0, Prescaler0, 2> {
        
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
            Timer0() = delete;
            Timer0(
                const WaveformMode0& waveformMode,
                const Prescaler0& p);

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
