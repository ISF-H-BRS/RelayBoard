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

#include "relaymanager.h"

#include <array>

// ---------------------------------------------------------------------------------------------- //

class UserPage
{
public:
    struct Data
    {
        std::array<float, RelayManager::RelayCount> voltageLimits;
        std::array<float, RelayManager::RelayCount> currentLimits;
    };

    class EraseError;
    class WriteError;

public:
    static void setData(const Data& data);
    static auto data() -> const Data&;
};

// ---------------------------------------------------------------------------------------------- //

class UserPage::EraseError : public std::exception
{
public:
    auto what() const noexcept -> const char* { return "ERASE_FAILED"; }
};

// ---------------------------------------------------------------------------------------------- //

class UserPage::WriteError : public std::exception
{
public:
    auto what() const noexcept -> const char* { return "WRITE_FAILED"; }
};

// ---------------------------------------------------------------------------------------------- //
