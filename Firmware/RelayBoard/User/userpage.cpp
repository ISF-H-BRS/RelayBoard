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

#include "userpage.h"

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr float MaximumVoltageLimit = RelayManager::MaximumVoltageLimit;
    constexpr float MaximumCurrentLimit = RelayManager::MaximumCurrentLimit;

    UserPage::Data g_data __attribute__((section(".userpage"))) =
    {
        {
            MaximumVoltageLimit, MaximumVoltageLimit, MaximumVoltageLimit, MaximumVoltageLimit,
            MaximumVoltageLimit, MaximumVoltageLimit, MaximumVoltageLimit, MaximumVoltageLimit,
            MaximumVoltageLimit, MaximumVoltageLimit, MaximumVoltageLimit, MaximumVoltageLimit,
            MaximumVoltageLimit, MaximumVoltageLimit, MaximumVoltageLimit, MaximumVoltageLimit
        },
        {
            MaximumCurrentLimit, MaximumCurrentLimit, MaximumCurrentLimit, MaximumCurrentLimit,
            MaximumCurrentLimit, MaximumCurrentLimit, MaximumCurrentLimit, MaximumCurrentLimit,
            MaximumCurrentLimit, MaximumCurrentLimit, MaximumCurrentLimit, MaximumCurrentLimit,
            MaximumCurrentLimit, MaximumCurrentLimit, MaximumCurrentLimit, MaximumCurrentLimit
        }
    };
}

// ---------------------------------------------------------------------------------------------- //

void UserPage::setData(const Data& data)
{
    static_assert(sizeof(data) % sizeof(uint64_t) == 0);

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef eraseInit = {};
    uint32_t sectorError = 0;

    eraseInit.TypeErase    = FLASH_TYPEERASE_PAGES;
    eraseInit.Banks        = FLASH_BANK_1;
    eraseInit.Page         = Config::UserPage;
    eraseInit.NbPages      = 1;

    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&eraseInit, &sectorError);

    if (status != HAL_OK)
    {
        HAL_FLASH_Lock();
        throw EraseError();
    }

    const auto buffer = reinterpret_cast<const uint64_t*>(&data);
    const auto flashStart = reinterpret_cast<uint32_t>(&g_data);

    const size_t wordCount = sizeof(data) / sizeof(uint64_t);

    for (size_t i = 0; i < wordCount; ++i)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                                   flashStart + i*sizeof(uint64_t), buffer[i]);
        if (status != HAL_OK)
            break;
    }

    HAL_FLASH_Lock();

    if (status != HAL_OK)
        throw WriteError();
}

// ---------------------------------------------------------------------------------------------- //

auto UserPage::data() -> const Data&
{
    return g_data;
}

// ---------------------------------------------------------------------------------------------- //
