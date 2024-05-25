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

#include "serialport.h"

#include <irb.h>

namespace irb::Private {

class IRB_EXPORT Device
{
public:
    enum class BootMode
    {
        Bootloader,
        Firmware
    };

    static constexpr std::chrono::milliseconds DefaultTimeout = SerialPort::DefaultTimeout;

public:
    Device(const char* port);
    ~Device();

    void reset();

    void setRelayState(size_t index, RelayState state);
    auto getRelayState(size_t index) const -> RelayState;

    auto getFaultMask() const -> uint16_t;

    void setStateMask(uint16_t mask);
    auto getStateMask() const -> uint16_t;

    auto getRelayPower(size_t index) const -> RelayPower;

    void setPowerLimit(size_t index, RelayPower power);
    auto getPowerLimit(size_t index) const -> RelayPower;

    void savePowerLimits();

    auto getBootMode() const -> BootMode;

    auto getBoardName() const -> std::string;
    auto getHardwareVersion() const -> std::string;

    // Firmware-specific
    auto getFirmwareVersion() const -> std::string;
    auto getSerialNumber() const -> std::string;
    auto getBuildTimestamp() const -> unsigned long;

    void launchBootloader();

    // Bootloader-specific
    auto getBootloaderVersion() const -> std::string;
    auto getSectorCount() const -> size_t;
    auto getFirmwareValid() const -> bool;

    void unlockFirmware();
    void lockFirmware();

    void eraseSector(size_t sector);
    void writeHexRecord(const std::string& record);

    void launchFirmware();

private:
    auto sendRequest(std::string request,
                     std::chrono::milliseconds timeout = DefaultTimeout) const -> std::string;

    void checkError(const std::string& response) const;

    auto parseString(const std::string& response,
                     const std::string& expectedTag) const -> std::string;

    auto parseULong(const std::string& response,
                    const std::string& expectedTag) const -> unsigned long;

    auto parseRelayState(const std::string& response,
                         const std::string& expectedTag) const -> RelayState;

    auto parseRelayPower(const std::string& response,
                         const std::string& expectedTag) const -> RelayPower;

    static auto mapError(const std::string& error) -> std::string;

private:
    SerialPort m_port;
};

} // End of namespace irb::Private
