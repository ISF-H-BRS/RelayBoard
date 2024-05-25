// ============================================================================================== //
//                                                                                                //
//  This file is part of the ISF RelayBoard project.                                              //
//                                                                                                //
//  Author:                                                                                       //
//  Marcel Hasler <mahasler@gmail.com>                                                            //
//                                                                                                //
//  Copyright (c) 2021 - 2023                                                                     //
//  Bonn-Rhein-Sieg University of Applied Sciences                                                //
//                                                                                                //
//  This program is free software: you can redistribute it and/or modify it under the terms       //
//  of the GNU General Public License as published by the Free Software Foundation, either        //
//  version 3 of the License, or (at your option) any later version.                              //
//                                                                                                //
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     //
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     //
//  See the GNU General Public License for more details.                                          //
//                                                                                                //
//  You should have received a copy of the GNU General Public License along with this program.    //
//  If not, see <https://www.gnu.org/licenses/>.                                                  //
//                                                                                                //
// ============================================================================================== //

#include "assert.h"
#include "hostinterface.h"

#include "usbd_cdc_if.h"

// ---------------------------------------------------------------------------------------------- //

HostInterface* HostInterface::s_instance = nullptr;

// ---------------------------------------------------------------------------------------------- //

HostInterface::HostInterface(Owner* owner)
    : m_owner(owner)
{
    ASSERT(m_owner != nullptr);

    s_instance = this;

    CDC_RegisterReceiveCallback(&HostInterface::cdcReceiveCallback);
    CDC_RegisterTxCompleteCallback(&HostInterface::cdcTxCompleteCallback);
}

// ---------------------------------------------------------------------------------------------- //

HostInterface::~HostInterface()
{
    CDC_RegisterReceiveCallback(nullptr);
    CDC_RegisterTxCompleteCallback(nullptr);

    s_instance = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::update()
{
    if (m_dataOverflow)
    {
        m_currentData.clear();
        m_dataOverflow = false;

        m_owner->onHostDataOverflow();
    }
    else if (m_dataReceived)
    {
        // New data may arrive before call to onHostDataReceived() returns
        const String data = m_currentData;

        m_currentData.clear();
        m_dataReceived = false;

        m_owner->onHostDataReceived(data);
    }
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::sendData(String data)
{
    data += "\r\n";
    auto buffer = reinterpret_cast<uint8_t*>(data.data());

    m_txComplete = false;
    CDC_Transmit(buffer, data.size());

    static constexpr int MaxWaitCycles = 10;

    for (int i = 0; i < MaxWaitCycles; ++i)
    {
        HAL_Delay(1);

        if (m_txComplete)
            break;
    }
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::processData(const uint8_t* data, uint32_t size)
{
    if (m_dataOverflow)
        return;

    if (m_dataReceived || m_currentData.size() + size > m_currentData.capacity())
        m_dataOverflow = true;
    else
    {
        for (uint32_t i = 0; i < size; ++i)
        {
            m_currentData += data[i];

            if (m_currentData.endsWith(LineTerminator))
            {
                m_currentData.trim(LineTerminatorSize);
                m_dataReceived = true;
                return;
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::cdcReceiveCallback(uint8_t* buffer, uint32_t size)
{
    ASSERT(s_instance != nullptr);
    s_instance->processData(buffer, size);
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::cdcTxCompleteCallback()
{
    ASSERT(s_instance != nullptr);
    s_instance->m_txComplete = true;
}

// ---------------------------------------------------------------------------------------------- //
