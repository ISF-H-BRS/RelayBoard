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

#include <irb.h>
using namespace irb;

// ---------------------------------------------------------------------------------------------- //

class Device::Private
{
public:
    Private(const char* port) : device(port)  {}
    irb::Private::Device device;
};

// ---------------------------------------------------------------------------------------------- //

Device::Device(const char* port)
    : d(std::make_unique<Private>(port))
{
    if (d->device.getBootMode() == irb::Private::Device::BootMode::Bootloader)
    {
        throw Error("The device is currently running in bootloader mode. "
                    "This may indicate a missing or corrupted firmware image. "
                    "Please use the firmware updater to upload a valid "
                    "firmware image to the device.");
    }
}

// ---------------------------------------------------------------------------------------------- //

Device::~Device() = default;

// ---------------------------------------------------------------------------------------------- //

void Device::reset()
{
    d->device.reset();
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getFaultMask() const -> uint16_t
{
    return d->device.getFaultMask();
}

// ---------------------------------------------------------------------------------------------- //

void Device::setRelayState(size_t index, RelayState state)
{
    d->device.setRelayState(index, state);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getRelayState(size_t index) const -> RelayState
{
    return d->device.getRelayState(index);
}

// ---------------------------------------------------------------------------------------------- //

void Device::setStateMask(uint16_t mask)
{
    d->device.setStateMask(mask);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getStateMask() const -> uint16_t
{
    return d->device.getStateMask();
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getRelayPower(size_t index) const -> RelayPower
{
    return d->device.getRelayPower(index);
}

// ---------------------------------------------------------------------------------------------- //

void Device::setPowerLimit(size_t index, RelayPower power)
{
    d->device.setPowerLimit(index, power);
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getPowerLimit(size_t index) const -> RelayPower
{
    return d->device.getPowerLimit(index);
}

// ---------------------------------------------------------------------------------------------- //

void Device::savePowerLimits()
{
    d->device.savePowerLimits();
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getHardwareVersion() const -> std::string
{
    return d->device.getHardwareVersion();
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getFirmwareVersion() const -> std::string
{
    return d->device.getFirmwareVersion();
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getSerialNumber() const -> std::string
{
    return d->device.getSerialNumber();
}

// ---------------------------------------------------------------------------------------------- //

auto Device::getBuildTimestamp() const -> unsigned long
{
    return d->device.getBuildTimestamp();
}

// ---------------------------------------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------- //

namespace {
    auto _irb_error() -> std::string&
    {
        static std::string error;
        return error;
    }

    template <typename Func, typename Cleanup>
    auto _irb_call(Func func, Cleanup cleanup) -> irb_result
    {
        try {
            func();
            _irb_error().clear();
            return IRB_RESULT_SUCCESS;
        }
        catch (const std::exception& e) {
            cleanup();
            _irb_error() = e.what();
            return IRB_RESULT_ERROR;
        }
    }
}

// ---------------------------------------------------------------------------------------------- //

struct _irb_device
{
    Private::Device device;
};

// ---------------------------------------------------------------------------------------------- //

irb_result irb_open_device(const char* port, irb_device** device)
{
    return _irb_call([&]{ *device = new _irb_device {{ port }}; },
                     [&]{ *device = nullptr; });
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_close_device(irb_device* device)
{
    delete device;
    return IRB_RESULT_SUCCESS;
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_reset_device(irb_device* device)
{
    return _irb_call([&]{ device->device.reset(); }, []{});
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_get_fault_mask(irb_device* device, uint16_t* mask)
{
    return _irb_call([&]{ *mask = device->device.getFaultMask(); },
                     [&]{ *mask = 0x0000; });
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_set_relay_state(irb_device* device, size_t index, irb_relay_state state)
{
    const auto func = [&]
    {
        const RelayState s = (state == IRB_RELAY_STATE_ON) ? RelayState::On : RelayState::Off;
        device->device.setRelayState(index, s);
    };

    return _irb_call(func, []{});
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_get_relay_state(irb_device* device, size_t index, irb_relay_state* state)
{
    const auto func = [&]
    {
        const RelayState s = device->device.getRelayState(index);
        *state = (s == RelayState::On) ? IRB_RELAY_STATE_ON : IRB_RELAY_STATE_OFF;
    };

    return _irb_call(func, [&]{ *state = IRB_RELAY_STATE_OFF; });
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_set_state_mask(irb_device* device, uint16_t mask)
{
    return _irb_call([&]{ device->device.setStateMask(mask); }, []{});
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_get_state_mask(irb_device* device, uint16_t* mask)
{
    return _irb_call([&]{ *mask = device->device.getStateMask(); },
                     [&]{ *mask = 0x0000; });
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_get_relay_power(irb_device* device, size_t index, irb_relay_power* power)
{
    const auto func = [&]
    {
        const RelayPower p = device->device.getRelayPower(index);
        *power = { p.voltage, p.current };
    };

    return _irb_call(func, [&]{ *power = { 0.0, 0.0 }; });
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_set_power_limit(irb_device* device, size_t index, irb_relay_power power)
{
    const auto func = [&]
    {
        const RelayPower p = { power.voltage, power.current };
        device->device.setPowerLimit(index, p);
    };

    return _irb_call(func, []{});
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_get_power_limit(irb_device* device, size_t index, irb_relay_power* power)
{
    const auto func = [&]
    {
        const RelayPower p = device->device.getPowerLimit(index);
        *power = { p.voltage, p.current };
    };

    return _irb_call(func, [&]{ *power = { 0.0, 0.0 }; });
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_save_power_limits(irb_device* device)
{
    return _irb_call([&]{ device->device.savePowerLimits(); }, []{});
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_get_hardware_version(irb_device* device, char buffer[])
{
    const auto func = [&]
    {
        const std::string version = device->device.getHardwareVersion();

        if (version.length() > IRB_VERSION_LENGTH)
            throw Error("The device returned in invalid hardware version string.");

        std::copy(version.begin(), version.end(), buffer);
        buffer[IRB_VERSION_LENGTH] = '\0';
    };

    return _irb_call(func, [&]{ buffer[0] = '\0'; });
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_get_firmware_version(irb_device* device, char buffer[])
{
    const auto func = [&]
    {
        const std::string version = device->device.getFirmwareVersion();

        if (version.length() > IRB_VERSION_LENGTH)
            throw Error("The device returned in invalid firmware version string.");

        std::copy(version.begin(), version.end(), buffer);
        buffer[IRB_VERSION_LENGTH] = '\0';
    };

    return _irb_call(func, [&]{ buffer[0] = '\0'; });
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_get_serial_number(irb_device* device, char buffer[])
{
    const auto func = [&]
    {
        const std::string serial = device->device.getSerialNumber();

        if (serial.length() > IRB_SERIAL_NUMBER_LENGTH)
            throw Error("The device returned in invalid serial number.");

        std::copy(serial.begin(), serial.end(), buffer);
        buffer[IRB_SERIAL_NUMBER_LENGTH] = '\0';
    };

    return _irb_call(func, [&]{ buffer[0] = '\0'; });
}

// ---------------------------------------------------------------------------------------------- //

irb_result irb_get_build_timestamp(irb_device* device, unsigned long* timestamp)
{
    return _irb_call([&]{ *timestamp = device->device.getBuildTimestamp(); },
                     [&]{ *timestamp = 0; });
}

// ---------------------------------------------------------------------------------------------- //

const char* irb_get_last_error(void)
{
    return _irb_error().c_str();
}

// ---------------------------------------------------------------------------------------------- //
