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

#include "main.h"

#include <exception>

class Ina226
{
public:
    enum class Address : uint8_t
    {
        A0  = 0b10000000,
        A1  = 0b10000010,
        A2  = 0b10000100,
        A3  = 0b10000110,
        A4  = 0b10001000,
        A5  = 0b10001010,
        A6  = 0b10001100,
        A7  = 0b10001110,
        A8  = 0b10010000,
        A9  = 0b10010010,
        A10 = 0b10010100,
        A11 = 0b10010110,
        A12 = 0b10011000,
        A13 = 0b10011010,
        A14 = 0b10011100,
        A15 = 0b10011110
    };

    static auto indexOf(Address address) {
        return (static_cast<uint8_t>(address) >> 1) - static_cast<uint8_t>(Address::A0);
    }

    enum class AverageCount : uint16_t
    {
        X1,
        X4,
        X16,
        X64,
        X128,
        X256,
        X512,
        X1024
    };

    static auto indexOf(AverageCount count) {
        return static_cast<uint16_t>(count);
    }

    enum class ConversionTime : uint16_t
    {
        _140us,
        _204us,
        _332us,
        _588us,
        _1100us,
        _2116us,
        _4156us,
        _8244us
    };

    static auto indexOf(ConversionTime time) {
        return static_cast<uint16_t>(time);
    }

    static constexpr int16_t DefaultCalibrationValue = 0;

    struct Configuration
    {
        AverageCount averageCount = AverageCount::X1;
        ConversionTime busVoltageConversionTime = ConversionTime::_1100us;
        ConversionTime shuntVoltageConversionTime = ConversionTime::_1100us;
        int16_t calibrationValue = DefaultCalibrationValue;
    };

    static constexpr float ShuntVoltageLsb = 2.5e-6F;
    static constexpr float BusVoltageLsb = 1.25e-3F;

    class WriteError : public std::exception
    {
    public:
        auto what() const noexcept -> const char* { return "INA226_WRITE_ERROR"; }
    };

    class ReadError : public std::exception
    {
    public:
        auto what() const noexcept -> const char* { return "INA226_READ_ERROR"; }
    };

public:
    Ina226(I2C_HandleTypeDef* i2c, Address address);

    auto address() const -> Address;

    void setConfiguration(const Configuration& config);
    auto getConfiguration() const -> Configuration;

    auto getShuntVoltage() const -> int16_t;
    auto getBusVoltage() const -> int16_t;
    auto getPower() const -> int16_t;
    auto getCurrent() const -> int16_t;

private:
    void writeRegister(uint8_t reg, uint16_t value);
    auto readRegister(uint8_t reg) const -> uint16_t;

private:
    I2C_HandleTypeDef* m_i2c;
    Address m_address;
};
