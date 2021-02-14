
#include "./Timer2.h"

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
    and aims at making the functionality and configuration of Timer2
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

        const WaveformMode2 WaveformMode2::Normal_Fixed = WaveformMode2::WaveformMode2(0x00, 0xFF),
                            WaveformMode2::PWM_PhaseCorrect_Fixed = WaveformMode2::WaveformMode2(0x01, 0xFF),
                            WaveformMode2::CTC_OCR2A = WaveformMode2::WaveformMode2(0x02, &OCR2A),
                            WaveformMode2::PWM_Fast_Fixed = WaveformMode2::WaveformMode2(0x03, 0xFF),
                            WaveformMode2::PWM_PhaseCorrect_OCR2A = WaveformMode2::WaveformMode2(0x05, &OCR2A),
                            WaveformMode2::PWM_Fast_OCR2A = WaveformMode2::WaveformMode2(0x07, &OCR2A);

        WaveformMode2::WaveformMode2(const uint8_t wgm, const uint8_t topv) : WaveformMode(wgm, topv)
        {
        }

        WaveformMode2::WaveformMode2(const uint8_t wgm, volatile uint8_t *const topp) : WaveformMode(wgm, topp)
        {
        }

        inline uint8_t WaveformMode2::TimerControlRegisterA() const
        {
            return m_mode & 0x03; //mask out all bits except the lowest 2 (WGM21:0)
        }

        inline uint8_t WaveformMode2::TimerControlRegisterB() const
        {
            return (m_mode & 0x04) << 1; //mask out all bits except the third bit and shift to position of 4th bit
        }

        namespace {
            uint8_t TimerControlRegisterB(const Prescaler2 &p)
            {
                return (((uint8_t)p) & 0x07); //mask out all bits except the lowest 3 (CS22:0)
            }
            
            uint8_t TimerControlRegisterA(const Channel& c, const CompareOutputMode& compMode)        {
                uint8_t bitOffset = c == Channel::A ? 6 : 4;
                return (((uint8_t)compMode) & 0x03) << bitOffset;
            }
        }

        Timer2::Timer2(const WaveformMode2 &waveformMode, const Prescaler2& prescaler) 
        :
        TemplateTimer(waveformMode, prescaler)
        {
            initCompareChannel(Channel::A, &OCR2A);
            initCompareChannel(Channel::B, &OCR2B);      
        }

        void Timer2::activate(const uint8_t &newTop, bool enOvflInt)
        {
            //reset all timer registers to stop any running config
            //also, we dont want to start with these registers because the timer will start running as soon as the prescaler values are set
            TCCR2A = 0; 
            TCCR2B = 0;             
            TCNT2 = 0; //reset counter
            TIMSK2 = 0; //reset all interrupts

            TIMSK2 = (enOvflInt & 0x1) << TOIE2;
            //if waveform mode allows it, set ICR1 or OCR1A
            updateTimerTop(newTop);
          

            //set the last missing values, starting the timer
            TCCR2A |= m_waveformMode.TimerControlRegisterA();
            TCCR2B = m_waveformMode.TimerControlRegisterB() | TimerControlRegisterB(m_prescaler);
        }

        /*
        * This only has an effect in non-PWM modes 
        */

        /* void Timer2::forceOutputCompareForChannel(const Channel2& m_channel) {
            uint8_t bitOffset = m_channel == Channel2::A ? 7 : 6;
            TCCR2B |= (1 << bitOffset);
        }
        */
        
        void Timer2::activateCompareChannel(const Channel& channel, const uint8_t& compareValue, const CompareOutputMode& outputMode, const bool enableInterrupt) {
            if(channel > Channel::B) return;
            TCCR2A |= TimerControlRegisterA(channel, outputMode);
            bool channelA = channel == Channel::A;
            TIMSK2 |= (enableInterrupt & 0x01) << (channelA ? OCIE2A : OCIE2B);

            updateCompareValue(channel, compareValue);
        }

        /*
            Configure the pin associated with the output compare channel as an output so it is usable as an output.
            For timer 2 this is pin PB3 (D11) for channel A or pin PD3 (D3) for channel B

        */
        void Timer2::activateOutputPin(const Channel& channel) {
            if(channel > Channel::B) return;
            if(channel == Channel::A) {
                PORTB &= ~(1 << PB3); //clear port b bit to make sure the pullup is off
                DDRB |= 1u << PB3; // set data direction register for pin to OUTPUT
            } else {
                PORTD &= ~(1 << PD3); //clear port b bit to make sure the pullup is off
                DDRD |= 1u << PD3; // set data direction register for pin to OUTPUT
            }
            
        }
    } // namespace Timer
} // namespace Atmega328Ppp
