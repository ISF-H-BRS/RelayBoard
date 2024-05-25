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

#include "hostinterface.h"
#include "relaymanager.h"

class RelayBoard : public HostInterface::Owner, public RelayManager::Owner
{
public:
	RelayBoard();

    void exec();

private:
    void onHostDataReceived(const String& data) override;
    void onHostDataOverflow() override;

    void onRelayFault() override;

    void protocolGetFaultMask();

    void protocolSetRelayState(const String& data, size_t tokenCount);
    void protocolGetRelayState(const String& data, size_t tokenCount);

    void protocolSetStateMask(const String& data, size_t tokenCount);
    void protocolGetStateMask();

    void protocolGetRelayPower(const String& data, size_t tokenCount);

    void protocolSetPowerLimit(const String& data, size_t tokenCount);
    void protocolGetPowerLimit(const String& data, size_t tokenCount);

    void protocolSavePowerLimits();

    void protocolReset();

    void protocolGetBootMode();

    void protocolGetBoardName();
    void protocolGetHardwareVersion();
    void protocolGetFirmwareVersion();
    void protocolGetSerialNumber();
    void protocolGetBuildTimestamp();

    void protocolLaunchBootloader();

    void checkTokenCount(size_t tokenCount, size_t expectedCount);

    void sendResponse(const String& tag, const String& data = {});
    void sendError(const String& error);

    auto toIndex(const String& s) -> uint8_t;
    auto toRelayState(const String& s) -> RelayState;

private:
    HostInterface m_hostInterface;
    RelayManager m_relayManager;
};
