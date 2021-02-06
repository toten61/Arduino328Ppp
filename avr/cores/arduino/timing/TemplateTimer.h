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
    This file is part of my (toten61) fork/modification of the official Arduino SDK.
    It uses the same license as the Official Arduino SDK: LGPL 2.1 or later
*/
namespace Atmega328Ppp {
    namespace Timer {

        template<typename RES>
        class WaveformMode {

        public: 
            const uint8_t m_mode;
            const RES m_topConstant; //if the waveform mode has a constant TOP value, it is saved here, otherwise 0x00
            volatile RES* const m_topPointer; //if the waveform mode has a non-constant TOP value, the address to the register is saved here

            bool operator==(const WaveformMode<RES>& b) const;
        protected:
            WaveformMode(const uint8_t wgm, const RES topv);
            WaveformMode(const uint8_t wgm, volatile RES *const topp);
        };

        

        enum class Channel {
            A,
            B,
            C,
            D
        };
    
        enum class CompareOutputMode {
            Disconnected = 0x00,
            Toggle = 0x01,
            Clear = 0x02,
            Set = 0x3
        };

        template <typename RES, typename WFM, typename PRE, unsigned int CHAN>
        class TemplateTimer {
        public:

            struct ChannelState {
                RES* m_compareRegister = nullptr;
                CompareOutputMode m_outputMode = CompareOutputMode::Disconnected;
            };

            TemplateTimer(const WFM& waveformMode, const PRE& prescaler);
            //set register values
            virtual void activate(const RES& timerTopValue, bool enOvflInt = false);
            virtual void activateCompareChannel(const Channel& channel, const RES& compareValue, const CompareOutputMode& outputMode, const bool enableInterrupt) = 0;
            virtual void activateOutputPin(const Channel& channel) = 0;
            void updateTimerTop(const RES& newTop);            
            void updateCompareValue(const Channel& channel, const RES& compareValue);
        protected:
            void initCompareChannel(const Channel& channel, RES* compareRegister);
            /*
            * This only has an effect in non-PWM modes 
            */
            
            WFM m_waveformMode;
            PRE m_prescaler;
            ChannelState m_channelStates[CHAN];
            //void forceOutputCompareForChannel(const Channel& m_channel);
        };

        template <typename RES>
        WaveformMode<RES>::WaveformMode(const uint8_t wgm, RES const topv) :
            m_mode(wgm),
            m_topConstant(topv),
            m_topPointer(nullptr)
        {    
        }

        template <typename RES>
        WaveformMode<RES>::WaveformMode(const uint8_t wgm, volatile RES *const topp) : 
            m_mode(wgm),
            m_topConstant(0x00),
            m_topPointer(topp)  
        {    
        }

        template <typename RES>
        bool WaveformMode<RES>::operator==(const WaveformMode<RES>& b) const {
            return m_mode == b.m_mode;
        }


        template<typename RES, typename WFM, typename PRE, unsigned int CHAN>
        TemplateTimer<RES, WFM, PRE, CHAN>::TemplateTimer(const WFM& waveformMode, const PRE& prescaler) 
        :
        m_waveformMode(waveformMode),
        m_prescaler(prescaler)
        {
            
        }

        template <typename RES, typename WFM, typename PRE, unsigned int CHAN>
        void TemplateTimer<RES, WFM, PRE, CHAN>::activate(const RES& newTop, bool enOvflInt) {
            updateTimerTop(newTop);
        }

        template <typename RES, typename WFM, typename PRE, unsigned int CHAN>
        void TemplateTimer<RES, WFM, PRE, CHAN>::updateTimerTop(const RES& newTop) {
            if(m_waveformMode.m_topPointer != nullptr) {
                *(m_waveformMode.m_topPointer) = newTop;
            }
        }

        template <typename RES, typename WFM, typename PRE, unsigned int CHAN>
        void TemplateTimer<RES, WFM, PRE, CHAN>::updateCompareValue(const Channel& channel, const RES& newTop) {
            
            uint8_t channelInd = (uint8_t) channel;
            if(channelInd > CHAN) return;

            *(m_channelStates[channelInd].m_compareRegister) = newTop;
        }

        template <typename RES, typename WFM, typename PRE, unsigned int CHAN>
        void TemplateTimer<RES, WFM, PRE, CHAN>::initCompareChannel(const Channel& channel, RES* compareRegister) {
            uint8_t channelInd = (uint8_t) channel;
            if(channelInd > CHAN) return;

            m_channelStates[channelInd].m_compareRegister = compareRegister;
        }
    }
}