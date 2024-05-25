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

#include <iostream>
#include <thread>

using namespace std::chrono_literals;

auto main(int argc, const char* argv[]) -> int
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return -1;
    }

    try {
        irb::Device device(argv[1]);

        std::cout << "Hardware version: " << device.getHardwareVersion() << std::endl;
        std::cout << "Firmware version: " << device.getFirmwareVersion() << std::endl;
        std::cout << "Serial number: " << device.getSerialNumber() << std::endl;
        std::cout << "Build timestamp: " << device.getBuildTimestamp() << std::endl;

        std::cout << "State mask: " << device.getStateMask() << std::endl;

        std::cout << "Setting state mask to 0xaaaa..." << std::endl;
        device.setStateMask(0xaaaa);

        std::cout << "New state mask: " << device.getStateMask() << std::endl;

        std::cout << "Setting state mask to 0x0000..." << std::endl;
        device.setStateMask(0x0000);

        std::cout << "New state mask: " << device.getStateMask() << std::endl;

        std::cout << "Toggling relay states..." << std::endl;

        for (size_t i = 0; i < irb::RelayCount; ++i)
        {
            device.setRelayState(i, irb::RelayState::On);
            std::this_thread::sleep_for(100ms);
        }

        for (size_t i = 0; i < irb::RelayCount; ++i)
        {
            device.setRelayState(i, irb::RelayState::Off);
            std::this_thread::sleep_for(100ms);
        }

        std::cout << "Relay powers:" << std::endl;

        for (size_t i = 0; i < irb::RelayCount; ++i)
        {
            const irb::RelayPower power = device.getRelayPower(i);
            std::cout << "    Relay " << i << ": "
                      << power.voltage << " V @ " << power.current << " A"
                      << std::endl;
        }

        std::cout << "Power limits:" << std::endl;

        for (size_t i = 0; i < irb::RelayCount; ++i)
        {
            const irb::RelayPower power = device.getPowerLimit(i);
            std::cout << "    Relay " << i << ": "
                      << power.voltage << " V @ " << power.current << " A" << std::endl;
        }

        std::cout << "Setting power limits to 16 V @ 1 A..." << std::endl;

        for (size_t i = 0; i < irb::RelayCount; ++i)
        {
            const irb::RelayPower power = { 16.0, 1.0 };
            device.setPowerLimit(i, power);
        }

        std::cout << "New power limits:" << std::endl;

        for (size_t i = 0; i < irb::RelayCount; ++i)
        {
            const irb::RelayPower power = device.getPowerLimit(i);
            std::cout << "    Relay " << i << ": "
                      << power.voltage << " V @ " << power.current << " A" << std::endl;
        }

        std::cout << "Saving power limits..." << std::endl;
        device.savePowerLimits();
        std::cout << "Done." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
