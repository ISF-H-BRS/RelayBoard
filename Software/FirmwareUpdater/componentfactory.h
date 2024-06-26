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

#include "device.h"

#include <FirmwareUpdater/Core/componentfactory.h>

class ComponentFactory : public FirmwareUpdater::ComponentFactory
{
public:
    static constexpr int DeviceUniqueId = 0;

public:
    ComponentFactory(std::unique_ptr<irb::Private::Device> device);

    auto getComponentList() const -> FirmwareUpdater::ComponentList override;

    auto getDescriptor(int uniqueId) const -> FirmwareUpdater::ComponentDescriptor override;
    auto getComponent(int uniqueId) const -> FirmwareUpdater::ComponentPtr override;

private:
    std::unique_ptr<irb::Private::Device> m_device;
};
