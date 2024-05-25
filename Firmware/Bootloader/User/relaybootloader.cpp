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

#include "config.h"
#include "relaybootloader.h"

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr char TokenSeparator = ' ';
}

// ---------------------------------------------------------------------------------------------- //

RelayBootloader::RelayBootloader()
    : m_hostInterface(this)
{
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::exec()
{
    HAL_GPIO_WritePin(STATUS_GPIO_Port, STATUS_Pin, GPIO_PIN_SET);

    while (true)
        m_hostInterface.update();
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::onHostDataReceived(const String& data)
{
    const size_t tokenCount = data.countTokens(TokenSeparator);

    if (tokenCount < 1)
        return;

    const String tag = data.getToken(TokenSeparator, 0);

    if (tag == "<ERASE_SECTOR>" || tag == "<WRITE_HEX_RECORD>")
    {
        if (tokenCount < 2)
            return sendError("MISSING_ARGUMENT");

        const String argument = data.getToken(TokenSeparator, 1);

        if (tag == "<ERASE_SECTOR>")
            protocolEraseSector(argument);
        else if (tag == "<WRITE_HEX_RECORD>")
            protocolWriteHexRecord(argument);
    }
    else if (tag == "<GET_BOOT_MODE>")
        protocolGetBootMode();
    else if (tag == "<GET_BOARD_NAME>")
        protocolGetBoardName();
    else if (tag == "<GET_HARDWARE_VERSION>")
        protocolGetHardwareVersion();
    else if (tag == "<GET_BOOTLOADER_VERSION>")
        protocolGetBootloaderVersion();
    else if (tag == "<GET_SECTOR_COUNT>")
        protocolGetSectorCount();
    else if (tag == "<GET_FIRMWARE_VALID>")
        protocolGetFirmwareValid();
    else if (tag == "<LAUNCH_FIRMWARE>")
        protocolLaunchFirmware();
    else if (tag == "<UNLOCK_FIRMWARE>")
        protocolUnlockFirmware();
    else if (tag == "<LOCK_FIRMWARE>")
        protocolLockFirmware();
    else
        sendError("UNKNOWN_COMMAND");
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::onHostDataOverflow()
{
    sendError("DATA_OVERFLOW");
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolGetBootMode()
{
    sendResponse("<BOOT_MODE>", "BOOTLOADER");
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolGetBoardName()
{
    sendResponse("<BOARD_NAME>", Config::BoardName);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolGetHardwareVersion()
{
    sendResponse("<HARDWARE_VERSION>", Config::HardwareVersion);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolGetBootloaderVersion()
{
    sendResponse("<BOOTLOADER_VERSION>", Config::BootloaderVersion);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolGetSectorCount()
{
    sendResponse("<SECTOR_COUNT>", String::format("%d", Config::FirmwareSectorCount));
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolGetFirmwareValid()
{
    const char* valid = Bootloader::getFirmwareValid() ? "1" : "0";
    sendResponse("<FIRMWARE_VALID>", valid);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolLaunchFirmware()
{
    sendResponse("<OK>");

    HAL_Delay(500); // Give host time to disconnect
    Bootloader::launchFirmware();
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolUnlockFirmware()
{
    try {
        Bootloader::unlockFirmware();
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolLockFirmware()
{
    try {
        Bootloader::lockFirmware();
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolEraseSector(const String& data)
{
    const auto sector = data.toULong();

    try {
        Bootloader::eraseSector(sector);
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::protocolWriteHexRecord(const String& data)
{
    try {
        Bootloader::writeHexRecord(data.c_str());
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::sendResponse(const String& tag, const String& data)
{
    auto response = tag;

    if (!data.empty())
        response += " " + data;

    m_hostInterface.sendData(response);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBootloader::sendError(const String& error)
{
    m_hostInterface.sendData("<ERROR> " + error);
}

// ---------------------------------------------------------------------------------------------- //
