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

#include "component.h"
#include "componentfactory.h"

#include <cassert>

// ---------------------------------------------------------------------------------------------- //

ComponentFactory::ComponentFactory(std::unique_ptr<irb::Private::Device> device)
    : m_device(std::move(device))
{
    assert(m_device != nullptr);
}

// ---------------------------------------------------------------------------------------------- //

auto ComponentFactory::getComponentList() const -> FirmwareUpdater::ComponentList
{
    FirmwareUpdater::ComponentList list = {
        getDescriptor(DeviceUniqueId)
    };

    return list;
}

// ---------------------------------------------------------------------------------------------- //

auto ComponentFactory::getDescriptor(int uniqueId) const -> FirmwareUpdater::ComponentDescriptor
{
    assert(uniqueId == DeviceUniqueId);
    return { uniqueId, "RelayBoard" };
}

// ---------------------------------------------------------------------------------------------- //

auto ComponentFactory::getComponent(int uniqueId) const -> FirmwareUpdater::ComponentPtr
{
    assert(uniqueId == DeviceUniqueId);
    return std::make_unique<Component>(m_device.get());
}

// ---------------------------------------------------------------------------------------------- //
