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

#include "config.h"
#include "powermonitor.h"

#include <array>

enum class RelayState
{
    Off,
    On
};

enum class RelayFault
{
    Unset,
    Set
};

class RelayManager
{
public:
    static constexpr size_t RelayCount = 16;

    static constexpr float MinimumVoltageLimit =  0.0F;
    static constexpr float MaximumVoltageLimit = 32.0F;

    static constexpr float MinimumCurrentLimit =  0.0F;
    static constexpr float MaximumCurrentLimit =  2.0F;

    class Owner
    {
        friend class RelayManager;
        virtual void onRelayFault() = 0;
    };

public:
    RelayManager(Owner* owner);
    ~RelayManager();

    void update();

    void reset();

    void setStateMask(uint16_t mask);
    auto getStateMask() const -> uint16_t;

    auto getFaultMask() const -> uint16_t;

    void setState(size_t index, RelayState state);
    auto getState(size_t index) const -> RelayState;

    auto getFault(size_t index) const -> RelayFault;

    auto getVoltage(size_t index) const -> float;
    auto getCurrent(size_t index) const -> float;

    void setVoltageLimit(size_t index, float voltage);
    auto getVoltageLimit(size_t index) const -> float;

    void setCurrentLimit(size_t index, float current);
    auto getCurrentLimit(size_t index) const -> float;

    void saveLimits();

private:
    void update(size_t index);

    void setFaultMask(uint16_t mask);
    void setFault(size_t index, RelayFault fault);

    static auto port(size_t index) -> GPIO_TypeDef*;
    static auto pin(size_t index) -> uint16_t;

private:
    Owner* m_owner;

    uint16_t m_faultMask = 0x0000;

    std::array<PowerMonitor, RelayCount> m_powerMonitors = {{
        { Config::PowerMonitorHandle, Ina226::Address::A0  },
        { Config::PowerMonitorHandle, Ina226::Address::A1  },
        { Config::PowerMonitorHandle, Ina226::Address::A2  },
        { Config::PowerMonitorHandle, Ina226::Address::A3  },
        { Config::PowerMonitorHandle, Ina226::Address::A4  },
        { Config::PowerMonitorHandle, Ina226::Address::A5  },
        { Config::PowerMonitorHandle, Ina226::Address::A6  },
        { Config::PowerMonitorHandle, Ina226::Address::A7  },
        { Config::PowerMonitorHandle, Ina226::Address::A8  },
        { Config::PowerMonitorHandle, Ina226::Address::A9  },
        { Config::PowerMonitorHandle, Ina226::Address::A10 },
        { Config::PowerMonitorHandle, Ina226::Address::A11 },
        { Config::PowerMonitorHandle, Ina226::Address::A12 },
        { Config::PowerMonitorHandle, Ina226::Address::A13 },
        { Config::PowerMonitorHandle, Ina226::Address::A14 },
        { Config::PowerMonitorHandle, Ina226::Address::A15 }
    }};

    std::array<float, RelayCount> m_voltages = {};
    std::array<float, RelayCount> m_currents = {};

    std::array<float, RelayCount> m_voltageLimits = {};
    std::array<float, RelayCount> m_currentLimits = {};
    bool m_limitsDirty = false;

    size_t m_updateIndex = 0;

    static RelayManager* s_instance;
};
