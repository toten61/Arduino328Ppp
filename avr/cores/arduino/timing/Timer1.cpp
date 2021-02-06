
#include "./Timer1.h"

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
    and aims at making the functionality and configuration of Timer1
    easier and more comfortable to use. It uses the same license 
    as the Official Arduino SDK: LGPL 2.1 or later
*/

namespace Atmega328Ppp
{

    namespace Timer
    {
        
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

        const WaveformMode1 WaveformMode1::Normal_Fixed = WaveformMode1::WaveformMode1(0x00, 0xFFFF),
                            WaveformMode1::PWM_PhaseCorrect_Fixed_8 = WaveformMode1::WaveformMode1(0x01, 0x00FF),
                            WaveformMode1::PWM_PhaseCorrect_Fixed_9 = WaveformMode1::WaveformMode1(0x02, 0x01FF),
                            WaveformMode1::PWM_PhaseCorrect_Fixed_10 = WaveformMode1::WaveformMode1(0x03, 0x03FF),
                            WaveformMode1::CTC_OCR1A = WaveformMode1::WaveformMode1(0x04, &OCR1A),
                            WaveformMode1::PWM_Fast_Fixed_8 = WaveformMode1::WaveformMode1(0x05, 0x00FF),
                            WaveformMode1::PWM_Fast_Fixed_9 = WaveformMode1::WaveformMode1(0x06, 0x01FF),
                            WaveformMode1::PWM_Fast_Fixed_10 = WaveformMode1::WaveformMode1(0x07, 0x03FF),
                            WaveformMode1::PWM_PhaseFreqCorrect_ICR1 = WaveformMode1::WaveformMode1(0x08, &ICR1),
                            WaveformMode1::PWM_PhaseFreqCorrect_OCR1A = WaveformMode1::WaveformMode1(0x09, &OCR1A),
                            WaveformMode1::PWM_PhaseCorrect_ICR1 = WaveformMode1::WaveformMode1(0x0A, &ICR1),
                            WaveformMode1::PWM_PhaseCorrect_OCR1A = WaveformMode1::WaveformMode1(0x0B, &OCR1A),
                            WaveformMode1::CTC_ICR1 = WaveformMode1::WaveformMode1(0x0C, &ICR1),
                            WaveformMode1::PWM_Fast_ICR1 = WaveformMode1::WaveformMode1(0x0E, &ICR1),
                            WaveformMode1::PWM_Fast_OCR1A = WaveformMode1::WaveformMode1(0x0F, &OCR1A);

        WaveformMode1::WaveformMode1(const uint8_t wgm, const uint16_t topv) : WaveformMode(wgm, topv)
        {
        }

        WaveformMode1::WaveformMode1(const uint8_t wgm, volatile uint16_t *const topp) : WaveformMode(wgm, topp)
        {
        }

        inline uint8_t WaveformMode1::TimerControlRegisterA() const
        {
            return m_mode & 0x03;
        }

        inline uint8_t WaveformMode1::TimerControlRegisterB() const
        {
            return (m_mode & 0x0C) << 1;
        }

        namespace {
            uint8_t TimerControlRegisterB(const Prescaler1 &p)
            {
                return (((uint8_t)p) & 0x07); //mask out all bits except the lowest 3 (CS12:0)
            }

            uint8_t TimerControlRegisterA(const Channel& c, const CompareOutputMode& compMode)        {
                uint8_t bitOffset = c == Channel::A ? 6 : 4;
                return (((uint8_t)compMode) & 0x03) << bitOffset;
            }
        }

        Timer1::Timer1(const WaveformMode1& waveformMode, const Prescaler1& prescaler)
        : 
        TemplateTimer(waveformMode, prescaler)
        {
            initCompareChannel(Channel::A, &OCR1A);
            initCompareChannel(Channel::B, &OCR1B);            
        }

        void Timer1::activate(const uint16_t& newTop, bool enOvflInt) {
            //reset all timer registers to stop any running config
            //also, we dont want to start with these registers because the timer will start running as soon as the prescaler values are set
            TCCR1A = 0; 
            TCCR1B = 0; 
            TCCR1C = 0;             
            TCNT1 = 0; //reset counter
            TIMSK1 = 0; //reset all interrupts


            TIMSK1 = (enOvflInt & 0x1) << TOIE1;
            //if waveform mode allows it, set ICR1 or OCR1A
            updateTimerTop(newTop);
          

            //set the last missing values, starting the timer
            TCCR1A |= m_waveformMode.TimerControlRegisterA();
            TCCR1B = m_waveformMode.TimerControlRegisterB() | TimerControlRegisterB(m_prescaler);
        }
        
        /*
        * This only has an effect in non-PWM modes 
        */

        /*void Timer1::forceOutputCompareForChannel(const Channel1& m_channel) {
        uint8_t bitOffset = m_channel == Channel1::A ? 7 : 6;
        TCCR1C |= (1 << bitOffset);
        }*/

        void Timer1::activateCompareChannel(const Channel& channel, const uint16_t& compareValue, const CompareOutputMode& outputMode, const bool enableInterrupt) {
            if(channel > Channel::B) return;
            TCCR1A |= TimerControlRegisterA(channel, outputMode);
            bool channelA = channel == Channel::A;
            TIMSK1 |= (enableInterrupt & 0x01) << (channelA ? OCIE1A : OCIE1B);

            updateCompareValue(channel, compareValue);
        }

        /*
            Configure the pin associated with the output compare channel as an output so it is usable as an output.
            For timer 1 this is pin PB1 (D9) or pin PB2 (D10)

        */
        void Timer1::activateOutputPin(const Channel& channel) {
            if(channel > Channel::B) return;
            uint8_t bitOffset = channel == Channel::A ? PB1 : PB2;
            PORTB &= ~(1u << bitOffset); //clear port b bit to make sure the pullup is off
            DDRB |= 1u << bitOffset; // set data direction register for pin to OUTPUT
        }
    } // namespace Timer
} // namespace Atmega328Ppp
