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

namespace Config {
    constexpr const char* BoardName = "RelayBoard";
    constexpr const char* HardwareVersion = "2.0";
    constexpr const char* BootloaderVersion = "1.0";

    constexpr uint32_t RamStartAddress = 0x20000004; // 32 bits for bootflags
    constexpr uint32_t RamEndAddress = 0x2000a000; // 40 kB

    constexpr uint32_t FlashStartAddress = FLASH_BASE;
    constexpr uint32_t FlashEndAddress = 0x08020000; // 128 kB

    constexpr uint32_t FirmwareStartSector = 24; // 24*2 kB bootloader space
    constexpr uint32_t FirmwareSectorCount = 40; // 78 kB firmware + 2 kB user page

    constexpr uint32_t FirmwareStartAddress = 0x0800c800; // Start of actual firmware
    constexpr uint32_t FirmwareEndAddress = FlashEndAddress - sizeof(uint64_t); // 32-bit CRC

    constexpr uint32_t ChecksumAddress = FirmwareEndAddress;

} // End of namespace Config
