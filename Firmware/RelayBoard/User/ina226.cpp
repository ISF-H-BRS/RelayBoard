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

#include "ina226.h"

#include <array>

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr uint32_t Timeout = 100;
}

namespace Register {
    constexpr uint8_t Configuration = 0x00;
    constexpr uint8_t ShuntVoltage  = 0x01;
    constexpr uint8_t BusVoltage    = 0x02;
    constexpr uint8_t Power         = 0x03;
    constexpr uint8_t Current       = 0x04;
    constexpr uint8_t Calibration   = 0x05;
    constexpr uint8_t EnableMask    = 0x06;
    constexpr uint8_t AlertLimit    = 0x07;
}

namespace Config {
    constexpr uint16_t AvgOffset    = 9;
    constexpr uint16_t VbusCtOffset = 6;
    constexpr uint16_t VshCtOffset  = 3;
    constexpr uint16_t ModeOffset   = 0;

    constexpr uint16_t AvgMask    = 0b0000111000000000;
    constexpr uint16_t VbusCtMask = 0b0000000111000000;
    constexpr uint16_t VshCtMask  = 0b0000000000111000;
    constexpr uint16_t ModeMask   = 0b0000000000000111;
}

// ---------------------------------------------------------------------------------------------- //

Ina226::Ina226(I2C_HandleTypeDef* i2c, Address address)
    : m_i2c(i2c),
      m_address(address)
{
}

// ---------------------------------------------------------------------------------------------- //

auto Ina226::address() const -> Address
{
    return m_address;
}

// ---------------------------------------------------------------------------------------------- //

void Ina226::setConfiguration(const Configuration& config)
{
    // Default value, continuous shunt and bus conversion
    static constexpr uint16_t mode = 0b111 << Config::ModeOffset;

    const uint16_t count = indexOf(config.averageCount) << Config::AvgOffset;
    const uint16_t bus = indexOf(config.busVoltageConversionTime) << Config::VbusCtOffset;
    const uint16_t shunt = indexOf(config.shuntVoltageConversionTime) << Config::VshCtOffset;

    const uint16_t value = count | bus | shunt | mode;
    writeRegister(Register::Configuration, value);

    writeRegister(Register::Calibration, config.calibrationValue);
}

// ---------------------------------------------------------------------------------------------- //

auto Ina226::getConfiguration() const -> Configuration
{
    const uint16_t configuration = readRegister(Register::Configuration);
    const uint16_t calibration = readRegister(Register::Calibration);

    return {
        static_cast<AverageCount>((configuration & Config::AvgMask) >> Config::AvgOffset),
        static_cast<ConversionTime>((configuration & Config::VbusCtMask) >> Config::VbusCtOffset),
        static_cast<ConversionTime>((configuration & Config::VshCtMask) >> Config::VshCtOffset),
        static_cast<int16_t>(calibration)
    };
}

// ---------------------------------------------------------------------------------------------- //

auto Ina226::getShuntVoltage() const -> int16_t
{
    const uint16_t value = readRegister(Register::ShuntVoltage);
    return reinterpret_cast<const int16_t&>(value);
}

// ---------------------------------------------------------------------------------------------- //

auto Ina226::getBusVoltage() const -> int16_t
{
    const uint16_t value = readRegister(Register::BusVoltage);
    return reinterpret_cast<const int16_t&>(value);
}

// ---------------------------------------------------------------------------------------------- //

auto Ina226::getPower() const -> int16_t
{
    const uint16_t value = readRegister(Register::Power);
    return reinterpret_cast<const int16_t&>(value);
}

// ---------------------------------------------------------------------------------------------- //

auto Ina226::getCurrent() const -> int16_t
{
    const uint16_t value = readRegister(Register::Current);
    return reinterpret_cast<const int16_t&>(value);
}

// ---------------------------------------------------------------------------------------------- //

void Ina226::writeRegister(uint8_t reg, uint16_t value)
{
    const auto address = static_cast<uint16_t>(m_address);

    std::array<uint8_t, 2> data = { static_cast<uint8_t>(value >> 8), static_cast<uint8_t>(value) };

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(m_i2c, address, reg, I2C_MEMADD_SIZE_8BIT,
                                                 data.data(), data.size(), Timeout);
    if (status != HAL_OK)
        throw WriteError();
}

// ---------------------------------------------------------------------------------------------- //

auto Ina226::readRegister(uint8_t reg) const -> uint16_t
{
    const auto address = static_cast<uint16_t>(m_address);

    std::array<uint8_t, 2> data;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(m_i2c, address, reg, I2C_MEMADD_SIZE_8BIT,
                                                data.data(), data.size(), Timeout);
    if (status != HAL_OK)
        throw ReadError();

    return (data[0] << 8) | data[1];
}

// ---------------------------------------------------------------------------------------------- //
