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

#ifdef _WIN32
  #ifdef IRB_BUILD_PROCESS
    #define IRB_EXPORT __declspec(dllexport)
  #else
    #define IRB_EXPORT __declspec(dllimport)
  #endif
#else
  #define IRB_EXPORT
#endif

// ---------------------------------------------------------------------------------------------- //
//  C++ API                                                                                       //
// ---------------------------------------------------------------------------------------------- //

#ifdef __cplusplus

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>

// ---------------------------------------------------------------------------------------------- //

namespace irb {

// ---------------------------------------------------------------------------------------------- //

constexpr size_t RelayCount = 16;

constexpr double MinimumVoltageLimit =  0.0;
constexpr double MaximumVoltageLimit = 32.0;

constexpr double MinimumCurrentLimit = 0.0;
constexpr double MaximumCurrentLimit = 2.0;

enum class RelayState
{
    Off,
    On
};

struct RelayPower
{
    double voltage;
    double current;
};

using Error = std::runtime_error;

// ---------------------------------------------------------------------------------------------- //

class IRB_EXPORT Device
{
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

    auto getHardwareVersion() const -> std::string;
    auto getFirmwareVersion() const -> std::string;
    auto getSerialNumber() const -> std::string;
    auto getBuildTimestamp() const -> unsigned long;

private:
    class Private;
    std::unique_ptr<Private> d;
};

// ---------------------------------------------------------------------------------------------- //

} // End of namespace irb

// ---------------------------------------------------------------------------------------------- //

#endif // __cplusplus


// ---------------------------------------------------------------------------------------------- //
//  C API                                                                                         //
// ---------------------------------------------------------------------------------------------- //

#ifdef __cplusplus
extern "C" {
#else
#include <stddef.h>
#include <stdint.h>
#endif

// ---------------------------------------------------------------------------------------------- //

#define IRB_RELAY_COUNT 16

#define IRB_MINIMUM_VOLTAGE_LIMIT  0.0
#define IRB_MAXIMUM_VOLTAGE_LIMIT 32.0

#define IRB_MINIMUM_CURRENT_LIMIT  0.0
#define IRB_MAXIMUM_CURRENT_LIMIT  2.0

#define IRB_VERSION_LENGTH 3
#define IRB_SERIAL_NUMBER_LENGTH 12

typedef enum {
    IRB_RESULT_SUCCESS,
    IRB_RESULT_ERROR
} irb_result;

typedef enum {
    IRB_RELAY_STATE_OFF,
    IRB_RELAY_STATE_ON
} irb_relay_state;

typedef struct {
    double voltage;
    double current;
} irb_relay_power;

typedef struct _irb_device irb_device;

// ---------------------------------------------------------------------------------------------- //

irb_result IRB_EXPORT irb_open_device(const char* port, irb_device** device);
irb_result IRB_EXPORT irb_close_device(irb_device* device);

irb_result IRB_EXPORT irb_reset_device(irb_device* device);

irb_result IRB_EXPORT irb_get_fault_mask(irb_device* device, uint16_t* mask);

irb_result IRB_EXPORT irb_set_relay_state(irb_device* device, size_t index, irb_relay_state state);
irb_result IRB_EXPORT irb_get_relay_state(irb_device* device, size_t index, irb_relay_state* state);

irb_result IRB_EXPORT irb_set_state_mask(irb_device* device, uint16_t mask);
irb_result IRB_EXPORT irb_get_state_mask(irb_device* device, uint16_t* mask);

irb_result IRB_EXPORT irb_get_relay_power(irb_device* device, size_t index, irb_relay_power* power);

irb_result IRB_EXPORT irb_set_power_limit(irb_device* device, size_t index, irb_relay_power power);
irb_result IRB_EXPORT irb_get_power_limit(irb_device* device, size_t index, irb_relay_power* power);

irb_result IRB_EXPORT irb_save_power_limits(irb_device* device);

irb_result IRB_EXPORT irb_get_hardware_version(irb_device* device, char buffer[]);
irb_result IRB_EXPORT irb_get_firmware_version(irb_device* device, char buffer[]);
irb_result IRB_EXPORT irb_get_serial_number(irb_device* device, char buffer[]);
irb_result IRB_EXPORT irb_get_build_timestamp(irb_device* device, unsigned long* timestamp);

const char* IRB_EXPORT irb_get_last_error(void);

// ---------------------------------------------------------------------------------------------- //

#ifdef __cplusplus
} // extern "C"
#endif

// ---------------------------------------------------------------------------------------------- //
