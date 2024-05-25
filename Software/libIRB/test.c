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

#include <irb.h>

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define CHECK_RESULT(result)                        \
if (result != IRB_RESULT_SUCCESS)                   \
{                                                   \
    fprintf(stderr, "%s\n", irb_get_last_error());  \
    return -1;                                      \
}                                                   \

static void sleep_ms(uint16_t milliseconds)
{
#ifdef _WIN32
    Sleep((DWORD)milliseconds);
#else
    uint16_t i;
    for (i = 0; i < milliseconds; ++i)
        usleep(1000);
#endif
}

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }

    irb_device *device;
    irb_result result = irb_open_device(argv[1], &device);
    CHECK_RESULT(result);

    char version[IRB_VERSION_LENGTH + 1];
    result = irb_get_hardware_version(device, version);
    CHECK_RESULT(result);

    printf("Hardware version: %s\n", version);

    result = irb_get_firmware_version(device, version);
    CHECK_RESULT(result);

    printf("Firmware version: %s\n", version);

    char serial[IRB_SERIAL_NUMBER_LENGTH + 1];
    result = irb_get_serial_number(device, serial);
    CHECK_RESULT(result);

    printf("Serial number: %s\n", serial);

    unsigned long timestamp;
    result = irb_get_build_timestamp(device, &timestamp);
    CHECK_RESULT(result);

    printf("Build timestamp: %lu\n", timestamp);

    uint16_t mask;
    result = irb_get_state_mask(device, &mask);
    CHECK_RESULT(result);

    printf("State mask: 0x%04x\n", mask);

    printf("Setting state mask to 0xaaaa...\n");
    result = irb_set_state_mask(device, 0xaaaa);
    CHECK_RESULT(result);

    result = irb_get_state_mask(device, &mask);
    CHECK_RESULT(result);

    printf("New state mask: 0x%04x\n", mask);

    printf("Setting state mask to 0x0000...\n");
    result = irb_set_state_mask(device, 0x0000);
    CHECK_RESULT(result);

    result = irb_get_state_mask(device, &mask);
    CHECK_RESULT(result);

    printf("New state mask: 0x%04x\n", mask);

    printf("Toggling relay states...\n");

    for (size_t i = 0; i < IRB_RELAY_COUNT; ++i)
    {
        result = irb_set_relay_state(device, i, IRB_RELAY_STATE_ON);
        sleep_ms(100);
    }

    for (size_t i = 0; i < IRB_RELAY_COUNT; ++i)
    {
        result = irb_set_relay_state(device, i, IRB_RELAY_STATE_OFF);
        sleep_ms(100);
    }

    printf("Relay powers:\n");

    for (size_t i = 0; i < IRB_RELAY_COUNT; ++i)
    {
        irb_relay_power power;

        result = irb_get_relay_power(device, i, &power);
        CHECK_RESULT(result);

        printf("    Relay %lu: %f V @ %f A\n", i, power.voltage, power.current);
    }

    return 0;
}
