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

#pragma once

#include "defaultstring.h"

#include <array>

class HostInterface
{
public:
    static constexpr const char* LineTerminator = "\r\n";
    static constexpr size_t LineTerminatorSize = 2;

    class Owner
    {
        friend class HostInterface;
        virtual void onHostDataReceived(const String& data) = 0;
        virtual void onHostDataOverflow() = 0;
    };

public:
    HostInterface(Owner* owner);
    ~HostInterface();

    void update();

    void sendData(String data);

protected:
    void processData(const uint8_t* data, uint32_t size);

    static void cdcReceiveCallback(uint8_t* buffer, uint32_t size);
    static void cdcTxCompleteCallback();

private:
    Owner* m_owner;
    String m_currentData;

    volatile bool m_dataReceived = false;
    volatile bool m_dataOverflow = false;

    volatile bool m_txComplete = true;
    static HostInterface* s_instance;
};
