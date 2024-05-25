####################################################################################################
#                                                                                                  #
#   This file is part of the ISF RelayBoard project.                                               #
#                                                                                                  #
#   Author:                                                                                        #
#   Marcel Hasler <mahasler@gmail.com>                                                             #
#                                                                                                  #
#   Copyright (c) 2021 - 2023                                                                      #
#   Bonn-Rhein-Sieg University of Applied Sciences                                                 #
#                                                                                                  #
#   This program is free software: you can redistribute it and/or modify it under the terms        #
#   of the GNU General Public License as published by the Free Software Foundation, either         #
#   version 3 of the License, or (at your option) any later version.                               #
#                                                                                                  #
#   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      #
#   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      #
#   See the GNU General Public License for more details.                                           #
#                                                                                                  #
#   You should have received a copy of the GNU General Public License along with this program.     #
#   If not, see <https:# www.gnu.org/licenses/>.                                                   #
#                                                                                                  #
####################################################################################################

cmake_minimum_required(VERSION 3.14)
project(IRB)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)

include_directories(include)

add_library(IRB SHARED
    include/irb.h
    device.cpp
    device.h
    irb.cpp
    serialport.cpp
    serialport.h
)

add_executable(test test.cpp)
target_link_libraries(test IRB)

if (WIN32)
    # For high-resolution timer
    set(_NT_TARGET_VERSION_WIN10_RS4 0xA000005)
    set_property(TARGET IRB APPEND PROPERTY COMPILE_DEFINITIONS _WIN32_WINNT=${_NT_TARGET_VERSION_WIN10_RS4})

    set_property(TARGET IRB APPEND PROPERTY COMPILE_DEFINITIONS IRB_BUILD_PROCESS)
    set_target_properties(IRB PROPERTIES PREFIX "")
endif()

add_executable(ctest test.c)
target_link_libraries(ctest IRB)
