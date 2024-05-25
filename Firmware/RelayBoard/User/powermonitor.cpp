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
#include "powermonitor.h"

// ---------------------------------------------------------------------------------------------- //

PowerMonitor::PowerMonitor(I2C_HandleTypeDef* i2c, Ina226::Address address)
    : m_chip(i2c, address)
{
    Ina226::Configuration config = {};
    config.averageCount = Ina226::AverageCount::X4;

    m_chip.setConfiguration(config);
}

// ---------------------------------------------------------------------------------------------- //

auto PowerMonitor::getVoltage() const -> float
{
    return m_chip.getBusVoltage() * Ina226::BusVoltageLsb;
}

// ---------------------------------------------------------------------------------------------- //

auto PowerMonitor::getCurrent() const -> float
{
    return m_chip.getShuntVoltage() * Ina226::ShuntVoltageLsb / Config::ShuntResistance;
}

// ---------------------------------------------------------------------------------------------- //
