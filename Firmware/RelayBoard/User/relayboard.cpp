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
#include "main.h"
#include "relayboard.h"
#include "timestamp.h"

#include "usbd_desc.h"

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr char TokenSeparator = ' ';

    constexpr uint32_t BootloaderMagic = 0xdeadbeef;
    volatile uint32_t g_bootloaderMagic __attribute__((section(".bootflags")));
}

// ---------------------------------------------------------------------------------------------- //

class MissingArgumentError : public std::exception
{
public:
    auto what() const noexcept -> const char* { return "MISSING_ARGUMENT"; }
};

// ---------------------------------------------------------------------------------------------- //

class InvalidArgumentError : public std::exception
{
public:
    auto what() const noexcept -> const char* { return "INVALID_ARGUMENT"; }
};

// ---------------------------------------------------------------------------------------------- //

RelayBoard::RelayBoard()
    : m_hostInterface(this),
      m_relayManager(this)
{
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::exec()
{
    HAL_GPIO_WritePin(STATUS_GPIO_Port, STATUS_Pin, GPIO_PIN_SET);

    while (true)
    {
        m_hostInterface.update();
        m_relayManager.update();
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::onHostDataReceived(const String& data)
{
    const size_t tokenCount = data.countTokens(TokenSeparator);

    if (tokenCount < 1)
        return;

    const String tag = data.getToken(TokenSeparator, 0);

    if (tag == "<GET_FAULT_MASK>")
        protocolGetFaultMask();
    else if (tag == "<SET_RELAY_STATE>")
        protocolSetRelayState(data, tokenCount);
    else if (tag == "<GET_RELAY_STATE>")
        protocolGetRelayState(data, tokenCount);
    else if (tag == "<SET_STATE_MASK>")
        protocolSetStateMask(data, tokenCount);
    else if (tag == "<GET_STATE_MASK>")
        protocolGetStateMask();
    else if (tag == "<GET_RELAY_POWER>")
        protocolGetRelayPower(data, tokenCount);
    else if (tag == "<SET_POWER_LIMIT>")
        protocolSetPowerLimit(data, tokenCount);
    else if (tag == "<GET_POWER_LIMIT>")
        protocolGetPowerLimit(data, tokenCount);
    else if (tag == "<SAVE_POWER_LIMITS>")
        protocolSavePowerLimits();
    else if (tag == "<RESET>")
        protocolReset();
    else if (tag == "<GET_BOOT_MODE>")
        protocolGetBootMode();
    else if (tag == "<GET_BOARD_NAME>")
        protocolGetBoardName();
    else if (tag == "<GET_HARDWARE_VERSION>")
        protocolGetHardwareVersion();
    else if (tag == "<GET_FIRMWARE_VERSION>")
        protocolGetFirmwareVersion();
    else if (tag == "<GET_SERIAL_NUMBER>")
        protocolGetSerialNumber();
    else if (tag == "<GET_BUILD_TIMESTAMP>")
        protocolGetBuildTimestamp();
    else if (tag == "<LAUNCH_BOOTLOADER>")
        protocolLaunchBootloader();
    else
        sendError("UNKNOWN_COMMAND");
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::onHostDataOverflow()
{
    sendError("DATA_OVERFLOW");
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::onRelayFault()
{
    HAL_GPIO_WritePin(STATUS_GPIO_Port, STATUS_Pin, GPIO_PIN_RESET);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetFaultMask()
{
    sendResponse(String::format("<FAULT_MASK> 0x%04x", m_relayManager.getFaultMask()));
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolSetRelayState(const String& data, size_t tokenCount)
{
    try {
        checkTokenCount(tokenCount, 3);

        const uint8_t index = toIndex(data.getToken(TokenSeparator, 1));
        const RelayState state = toRelayState(data.getToken(TokenSeparator, 2));

        m_relayManager.setState(index, state);
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetRelayState(const String& data, size_t tokenCount)
{
    try {
        checkTokenCount(tokenCount, 2);

        const uint8_t index = toIndex(data.getToken(TokenSeparator, 1));
        const RelayState state = m_relayManager.getState(index);

        sendResponse("<RELAY_STATE>", (state == RelayState::On) ? "ON" : "OFF");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolSetStateMask(const String& data, size_t tokenCount)
{
    try {
        checkTokenCount(tokenCount, 2);

        const auto mask = data.getToken(TokenSeparator, 1).toULong();

        if (mask > 0xffff)
            throw InvalidArgumentError();

        m_relayManager.setStateMask(mask);
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetStateMask()
{
    sendResponse(String::format("<STATE_MASK> 0x%04x", m_relayManager.getStateMask()));
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetRelayPower(const String& data, size_t tokenCount)
{
    try {
        checkTokenCount(tokenCount, 2);

        const uint8_t index = toIndex(data.getToken(TokenSeparator, 1));

        const float voltage = m_relayManager.getVoltage(index);
        const float current = m_relayManager.getCurrent(index);

        sendResponse("<RELAY_POWER>", String::format("%.2f,%.3f", voltage, current));
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolSetPowerLimit(const String& data, size_t tokenCount)
{
    try {
        checkTokenCount(tokenCount, 3);

        const uint8_t index = toIndex(data.getToken(TokenSeparator, 1));

        const String power = data.getToken(TokenSeparator, 2);
        const size_t powerTokenCount = power.countTokens(',');

        checkTokenCount(powerTokenCount, 2);

        const float voltage = power.getToken(',', 0).toFloat();
        const float current = power.getToken(',', 1).toFloat();

        const bool valid = voltage >= RelayManager::MinimumVoltageLimit &&
                           voltage <= RelayManager::MaximumVoltageLimit &&
                           current >= RelayManager::MinimumCurrentLimit &&
                           current <= RelayManager::MaximumCurrentLimit;
        if (!valid)
            throw InvalidArgumentError();

        m_relayManager.setVoltageLimit(index, voltage);
        m_relayManager.setCurrentLimit(index, current);

        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetPowerLimit(const String& data, size_t tokenCount)
{
    try {
        checkTokenCount(tokenCount, 2);

        const uint8_t index = toIndex(data.getToken(TokenSeparator, 1));

        const float voltage = m_relayManager.getVoltageLimit(index);
        const float current = m_relayManager.getCurrentLimit(index);

        sendResponse("<POWER_LIMIT>", String::format("%.2f,%.3f", voltage, current));
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolSavePowerLimits()
{
    try {
        m_relayManager.saveLimits();
        sendResponse("<OK>");
    }
    catch (const std::exception& e) {
        sendError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolReset()
{
    m_relayManager.reset();
    HAL_GPIO_WritePin(STATUS_GPIO_Port, STATUS_Pin, GPIO_PIN_SET);
    sendResponse("<OK>");
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetBootMode()
{
    sendResponse("<BOOT_MODE>", "FIRMWARE");
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetBoardName()
{
    sendResponse("<BOARD_NAME>", Config::BoardName);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetHardwareVersion()
{
    sendResponse("<HARDWARE_VERSION>", Config::HardwareVersion);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetFirmwareVersion()
{
    sendResponse("<FIRMWARE_VERSION>", Config::FirmwareVersion);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetSerialNumber()
{
    uint16_t size = 0;
    const uint8_t* buffer = FS_Desc.GetSerialStrDescriptor(USBD_SPEED_FULL, &size);

    String serial;

    // Stored as UTF-16, but contains only ASCII.
    // The first two bytes contain size and type.
    for (size_t i = 2; i < size; i += 2)
        serial += buffer[i];

    sendResponse("<SERIAL_NUMBER>", serial);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolGetBuildTimestamp()
{
    sendResponse("<BUILD_TIMESTAMP>", String::format("%d", BUILD_TIMESTAMP));
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::protocolLaunchBootloader()
{
    sendResponse("<OK>");

    HAL_Delay(500);

    g_bootloaderMagic = BootloaderMagic;
    HAL_NVIC_SystemReset();
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::checkTokenCount(size_t tokenCount, size_t expectedCount)
{
    if (tokenCount < expectedCount)
        throw MissingArgumentError();
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::sendResponse(const String& tag, const String& data)
{
    auto response = tag;

    if (!data.empty())
        response += " " + data;

    m_hostInterface.sendData(response);
}

// ---------------------------------------------------------------------------------------------- //

void RelayBoard::sendError(const String& error)
{
    m_hostInterface.sendData("<ERROR> " + error);
}

// ---------------------------------------------------------------------------------------------- //

auto RelayBoard::toIndex(const String& s) -> uint8_t
{
    if (s.empty())
        throw InvalidArgumentError();

    long index = s.toLong();

    if (index < 0 || index >= 16)
        throw InvalidArgumentError();

    return static_cast<uint8_t>(index);
}

// ---------------------------------------------------------------------------------------------- //

auto RelayBoard::toRelayState(const String& s) -> RelayState
{
    if (s == "ON")
        return RelayState::On;

    if (s == "OFF")
        return RelayState::Off;

    throw InvalidArgumentError();
}

// ---------------------------------------------------------------------------------------------- //
