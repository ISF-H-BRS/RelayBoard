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

#include "assert.h"
#include "relaymanager.h"
#include "userpage.h"

#include <algorithm>

// ---------------------------------------------------------------------------------------------- //

RelayManager* RelayManager::s_instance = nullptr;

// ---------------------------------------------------------------------------------------------- //

RelayManager::RelayManager(Owner* owner)
    : m_owner(owner)
{
    ASSERT(owner != nullptr);

    ASSERT(s_instance == nullptr);
    s_instance = this;

    const UserPage::Data& data = UserPage::data();

    m_voltageLimits = data.voltageLimits;
    m_currentLimits = data.currentLimits;

    for (auto& voltage : m_voltageLimits)
        voltage = std::clamp(voltage, MinimumVoltageLimit, MaximumVoltageLimit);

    for (auto& current : m_currentLimits)
        current = std::clamp(current, MinimumCurrentLimit, MaximumCurrentLimit);
}

// ---------------------------------------------------------------------------------------------- //

RelayManager::~RelayManager()
{
    s_instance = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

void RelayManager::update()
{
    update(m_updateIndex);

    if (++m_updateIndex >= RelayCount)
        m_updateIndex = 0;
}

// ---------------------------------------------------------------------------------------------- //

void RelayManager::update(size_t index)
{
    if (getFault(index) == RelayFault::Set)
    {
        ASSERT(getState(index) == RelayState::Off);
        return;
    }

    static constexpr int RetryCount = 3;

    for (int i = 1; i <= RetryCount; ++i)
    {
        try {
            m_voltages[index] = m_powerMonitors[index].getVoltage();
            m_currents[index] = m_powerMonitors[index].getCurrent();

            const bool valid = m_voltages[index] <= m_voltageLimits[index] &&
                               m_currents[index] <= m_currentLimits[index];
            if (valid)
                return;

            break;
        }
        catch (...) {
        }
    }

    setState(index, RelayState::Off);
    setFault(index, RelayFault::Set);

    m_owner->onRelayFault();
}

// ---------------------------------------------------------------------------------------------- //

void RelayManager::reset()
{
    setStateMask(0x0000);
    setFaultMask(0x0000);
}

// ---------------------------------------------------------------------------------------------- //

void RelayManager::setStateMask(uint16_t mask)
{
    for (size_t i = 0; i < RelayCount; ++i)
        setState(i, (mask & (1<<i)) ? RelayState::On : RelayState::Off);
}

// ---------------------------------------------------------------------------------------------- //

auto RelayManager::getStateMask() const -> uint16_t
{
    uint16_t mask = 0x0000;

    for (size_t i = 0; i < RelayCount; ++i)
    {
        const uint16_t bit = (getState(i) == RelayState::On) ? 1 : 0;
        mask |= (bit<<i);
    }

    return mask;
}

// ---------------------------------------------------------------------------------------------- //

void RelayManager::setFaultMask(uint16_t mask)
{
    m_faultMask = mask;
}

// ---------------------------------------------------------------------------------------------- //

auto RelayManager::getFaultMask() const -> uint16_t
{
    return m_faultMask;
}

// ---------------------------------------------------------------------------------------------- //

void RelayManager::setState(size_t index, RelayState state)
{
    ASSERT(index < RelayCount);

    if (m_faultMask & (1<<index))
        return;

    const GPIO_PinState pinState = (state == RelayState::On) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(port(index), pin(index), pinState);
}

// ---------------------------------------------------------------------------------------------- //

auto RelayManager::getState(size_t index) const -> RelayState
{
    ASSERT(index < RelayCount);

    const GPIO_PinState state = HAL_GPIO_ReadPin(port(index), pin(index));
    return (state == GPIO_PIN_SET) ? RelayState::On : RelayState::Off;
}

// ---------------------------------------------------------------------------------------------- //

void RelayManager::setFault(size_t index, RelayFault fault)
{
    ASSERT(index < RelayCount);

    if (fault == RelayFault::Set)
        m_faultMask |= (1<<index);
    else
        m_faultMask &= ~(1<<index);
}

// ---------------------------------------------------------------------------------------------- //

auto RelayManager::getFault(size_t index) const -> RelayFault
{
    return (m_faultMask & (1<<index)) ? RelayFault::Set : RelayFault::Unset;
}

// ---------------------------------------------------------------------------------------------- //

auto RelayManager::getVoltage(size_t index) const -> float
{
    ASSERT(index < RelayCount);
    return m_voltages[index];
}

// ---------------------------------------------------------------------------------------------- //

auto RelayManager::getCurrent(size_t index) const -> float
{
    ASSERT(index < RelayCount);
    return m_currents[index];
}

// ---------------------------------------------------------------------------------------------- //

void RelayManager::setVoltageLimit(size_t index, float voltage)
{
    ASSERT(index < RelayCount);

    if (voltage != m_voltageLimits[index])
    {
        m_voltageLimits[index] = voltage;
        m_limitsDirty = true;
    }
}

// ---------------------------------------------------------------------------------------------- //

auto RelayManager::getVoltageLimit(size_t index) const -> float
{
    ASSERT(index < RelayCount);
    return m_voltageLimits[index];
}

// ---------------------------------------------------------------------------------------------- //

void RelayManager::setCurrentLimit(size_t index, float current)
{
    ASSERT(index < RelayCount);

    if (current != m_currentLimits[index])
    {
        m_currentLimits[index] = current;
        m_limitsDirty = true;
    }
}

// ---------------------------------------------------------------------------------------------- //

auto RelayManager::getCurrentLimit(size_t index) const -> float
{
    ASSERT(index < RelayCount);
    return m_currentLimits[index];
}

// ---------------------------------------------------------------------------------------------- //

void RelayManager::saveLimits()
{
    if (m_limitsDirty)
    {
        const UserPage::Data data = {
            m_voltageLimits, m_currentLimits
        };

        UserPage::setData(data);
        m_limitsDirty = false;
    }
}

// ---------------------------------------------------------------------------------------------- //

auto RelayManager::port(size_t index) -> GPIO_TypeDef*
{
    ASSERT(index < RelayCount);

    static const std::array<GPIO_TypeDef*, RelayCount> ports = {
        RELAY01_GPIO_Port, RELAY02_GPIO_Port, RELAY03_GPIO_Port, RELAY04_GPIO_Port,
        RELAY05_GPIO_Port, RELAY06_GPIO_Port, RELAY07_GPIO_Port, RELAY08_GPIO_Port,
        RELAY09_GPIO_Port, RELAY10_GPIO_Port, RELAY11_GPIO_Port, RELAY12_GPIO_Port,
        RELAY13_GPIO_Port, RELAY14_GPIO_Port, RELAY15_GPIO_Port, RELAY16_GPIO_Port
    };

    return ports[index];
}

// ---------------------------------------------------------------------------------------------- //

auto RelayManager::pin(size_t index) -> uint16_t
{
    ASSERT(index < RelayCount);

    static const std::array<uint16_t, RelayCount> pins = {
        RELAY01_Pin, RELAY02_Pin, RELAY03_Pin, RELAY04_Pin,
        RELAY05_Pin, RELAY06_Pin, RELAY07_Pin, RELAY08_Pin,
        RELAY09_Pin, RELAY10_Pin, RELAY11_Pin, RELAY12_Pin,
        RELAY13_Pin, RELAY14_Pin, RELAY15_Pin, RELAY16_Pin
    };

    return pins[index];
}

// ---------------------------------------------------------------------------------------------- //
