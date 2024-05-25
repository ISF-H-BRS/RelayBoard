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

#include "device.h"
using namespace irb::Private;

#include <sstream>

// ---------------------------------------------------------------------------------------------- //

namespace {
    auto split(const std::string& s, char delim) -> std::vector<std::string>
    {
        std::vector<std::string> result;

        std::istringstream stream(s);
        std::string token;

        while (std::getline(stream, token, delim))
            result.push_back(token);

        if (s.back() == delim)
            result.push_back("");

        return result;
    }

    auto operator<<(std::ostream& os, irb::RelayState state) -> std::ostream&
    {
        os << ((state == irb::RelayState::On) ? "ON" : "OFF");
        return os;
    }

    auto operator>>(std::istream& is, irb::RelayState& state) -> std::istream&
    {
        std::string s;
        is >> s;

        if (s == "ON")
            state = irb::RelayState::On;
        else if (s == "OFF")
            state = irb::RelayState::Off;
        else
            is.setstate(std::ios::failbit);

        return is;
    }

    template <typename T>
    auto to(const std::string& s) -> T
    {
        std::istringstream stream(s);

        T t = {};
        stream >> t;

        if (stream.fail())
            throw std::exception();

        return t;
    }

    template <typename T>
    auto toString(T value) -> std::string
    {
        std::ostringstream stream;
        stream << value;
        return stream.str();
    }
}

// ---------------------------------------------------------------------------------------------- //

class InvalidResponseError : public irb::Error
{
public:
    InvalidResponseError(const std::string& response)
        : irb::Error("Invalid response received from device: '" + response + "'") {}
};

// ---------------------------------------------------------------------------------------------- //

Device::Device(const char* port)
    : m_port(port)
{
}

// ---------------------------------------------------------------------------------------------- //

Device::~Device() = default;

// ---------------------------------------------------------------------------------------------- //

void Device::reset()
{
    const std::string response = sendRequest("<RESET>");

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

void Device::setRelayState(size_t index, RelayState state)
{
    const std::string request = "<SET_RELAY_STATE> " + toString(index) + " " + toString(state);
    const std::string response = sendRequest(request);

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getRelayState(size_t index) const -> RelayState
{
    const std::string response = sendRequest("<GET_RELAY_STATE> " + toString(index));
    return parseRelayState(response, "<RELAY_STATE>");
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getFaultMask() const -> uint16_t
{
    const std::string response = sendRequest("<GET_FAULT_MASK>");
    return parseULong(response, "<FAULT_MASK>");
}

// ---------------------------------------------------------------------------------------------- //

void Device::setStateMask(uint16_t mask)
{
    const std::string response = sendRequest("<SET_STATE_MASK> " + toString(mask));

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getStateMask() const -> uint16_t
{
    const std::string response = sendRequest("<GET_STATE_MASK>");
    return parseULong(response, "<STATE_MASK>");
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getRelayPower(size_t index) const -> RelayPower
{
    const std::string response = sendRequest("<GET_RELAY_POWER> " + toString(index));
    return parseRelayPower(response, "<RELAY_POWER>");
}

// ---------------------------------------------------------------------------------------------- //

void Device::setPowerLimit(size_t index, RelayPower power)
{
    if (power.voltage < irb::MinimumVoltageLimit || power.voltage > irb::MaximumVoltageLimit)
        throw irb::Error("Invalid argument for voltage limit.");

    if (power.current < irb::MinimumCurrentLimit || power.current > irb::MaximumCurrentLimit)
        throw irb::Error("Invalid argument for current limit.");

    const std::string response = sendRequest("<SET_POWER_LIMIT> " + toString(index) + " "
                                        + toString(power.voltage) + "," + toString(power.current));
    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getPowerLimit(size_t index) const -> RelayPower
{
    const std::string response = sendRequest("<GET_POWER_LIMIT> " + toString(index));
    return parseRelayPower(response, "<POWER_LIMIT>");
}

// ---------------------------------------------------------------------------------------------- //

void Device::savePowerLimits()
{
    const auto timeout = 1s;
    const std::string response = sendRequest("<SAVE_POWER_LIMITS>", timeout);

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getBootMode() const -> BootMode
{
    const std::string response = sendRequest("<GET_BOOT_MODE>");
    const std::string mode = parseString(response, "<BOOT_MODE>");

    if (mode == "BOOTLOADER")
        return BootMode::Bootloader;

    if (mode == "FIRMWARE")
        return BootMode::Firmware;

    throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getBoardName() const -> std::string
{
    const std::string response = sendRequest("<GET_BOARD_NAME>");
    return parseString(response, "<BOARD_NAME>");
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getHardwareVersion() const -> std::string
{
    const std::string response = sendRequest("<GET_HARDWARE_VERSION>");
    return parseString(response, "<HARDWARE_VERSION>");
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getFirmwareVersion() const -> std::string
{
    const std::string response = sendRequest("<GET_FIRMWARE_VERSION>");
    return parseString(response, "<FIRMWARE_VERSION>");
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getSerialNumber() const -> std::string
{
    const std::string response = sendRequest("<GET_SERIAL_NUMBER>");
    return parseString(response, "<SERIAL_NUMBER>");
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getBuildTimestamp() const -> unsigned long
{
    const std::string response = sendRequest("<GET_BUILD_TIMESTAMP>");
    return parseULong(response, "<BUILD_TIMESTAMP>");
}

// ---------------------------------------------------------------------------------------------- //

void Device::launchBootloader()
{
    const std::string response = sendRequest("<LAUNCH_BOOTLOADER>");

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getBootloaderVersion() const -> std::string
{
    const std::string response = sendRequest("<GET_BOOTLOADER_VERSION>");
    return parseString(response, "<BOOTLOADER_VERSION>");
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getSectorCount() const -> size_t
{
    const std::string response = sendRequest("<GET_SECTOR_COUNT>");
    return parseULong(response, "<SECTOR_COUNT>");
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getFirmwareValid() const -> bool
{
    const std::string response = sendRequest("<GET_FIRMWARE_VALID>");
    return parseULong(response, "<FIRMWARE_VALID>");
}

// ---------------------------------------------------------------------------------------------- //

void Device::unlockFirmware()
{
    const std::string response = sendRequest("<UNLOCK_FIRMWARE>");

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

void Device::lockFirmware()
{
    const std::string response = sendRequest("<LOCK_FIRMWARE>");

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

void Device::eraseSector(size_t sector)
{
    const auto timeout = 2s;
    const std::string response = sendRequest("<ERASE_SECTOR> " + toString(sector), timeout);

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

void Device::writeHexRecord(const std::string& record)
{
    const auto timeout = 1s;
    const std::string response = sendRequest("<WRITE_HEX_RECORD> " + record, timeout);

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

void Device::launchFirmware()
{
    const std::string response = sendRequest("<LAUNCH_FIRMWARE>");

    if (response != "<OK>")
        throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::sendRequest(std::string request,
                         std::chrono::milliseconds timeout) const -> std::string
{
    request += "\r\n";
    m_port.sendData(std::vector<uint8_t>(request.begin(), request.end()));

    const bool dataAvailable = m_port.waitForDataAvailable(timeout);

    if (!dataAvailable)
        throw Error("Request timed out.");

    const std::vector<uint8_t> data = m_port.readAllData();
    std::string response(data.begin(), data.end());

    static const std::string lineBreak = "\r\n";

    if (!response.ends_with(lineBreak))
        throw InvalidResponseError(response);

    response.erase(response.size() - lineBreak.size());

    checkError(response);
    return response;
}

// ---------------------------------------------------------------------------------------------- //

void Device::checkError(const std::string& response) const
{
    const std::string tag = response.substr(0, response.find_first_of(' '));

    if (tag == "<ERROR>")
    {
        const std::string error = response.substr(tag.length() + 1);
        throw Error(mapError(error));
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Device::parseString(const std::string& response,
                         const std::string& expectedTag) const -> std::string
{
    const std::vector<std::string> tokens = split(response, ' ');

    if (tokens.size() == 2 && tokens.at(0) == expectedTag)
        return tokens.at(1);

    throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::parseULong(const std::string& response,
                        const std::string& expectedTag) const -> unsigned long
{
    const std::string value = parseString(response, expectedTag);

    try {
        return std::stoul(value, nullptr, 0);
    }
    catch (...) {
    }

    throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::parseRelayState(const std::string& response,
                             const std::string& expectedTag) const -> RelayState
{
    const std::string state = parseString(response, expectedTag);

    try {
        return to<RelayState>(state);
    }
    catch (...) {
    }

    throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::parseRelayPower(const std::string& response,
                             const std::string& expectedTag) const -> RelayPower
{
    const std::string power = parseString(response, expectedTag);

    const std::vector<std::string> values = split(power, ',');

    if (values.size() == 2)
    {
        try {
            return {
                to<double>(values.at(0)), to<double>(values.at(1))
            };
        }
        catch (...) {
        }
    }

    throw InvalidResponseError(response);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::mapError(const std::string& error) -> std::string
{
    if (error == "DATA_OVERFLOW")
        return "Data overflow.";

    if (error == "UNKNOWN_COMMAND")
        return "Unknown command.";

    if (error == "MISSING_ARGUMENT")
        return "Missing argument.";

    if (error == "INVALID_ARGUMENT")
        return "Invalid argument.";

    if (error == "INVALID_ADDRESS")
        return "Invalid address.";

    if (error == "INVALID_LENGTH")
        return "Invalid length.";

    if (error == "ERASE_FAILED")
        return "Erase failed.";

    if (error == "WRITE_FAILED")
        return "Write failed.";

    if (error == "DATA_MISMATCH")
        return "Data mismatch.";

    return "Unknown error code received: " + error;
}

// ---------------------------------------------------------------------------------------------- //
