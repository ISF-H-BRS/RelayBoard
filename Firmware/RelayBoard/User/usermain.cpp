/********************************************************
 *                                                      *
 *  This file is part of the RelayBoard project.        *
 *                                                      *
 *  Author:                                             *
 *  Marcel Hasler <marcel.hasler@h-brs.de>              *
 *                                                      *
 *  (c) Bonn-Rhein-Sieg University of Applied Sciences  *
 *                                                      *
 ********************************************************/

#include "main.h"
#include "relayboard.h"
#include "usermain.h"

#include <array>
#include <new>

// ---------------------------------------------------------------------------------------------- //

namespace __gnu_cxx
{
    // Strip unused name-demangling code from libstdc++
    void __verbose_terminate_handler() { ASSERT(false); }
}

// ---------------------------------------------------------------------------------------------- //

namespace {
    std::array<char, sizeof(RelayBoard)> g_relayBuffer;
}

// ---------------------------------------------------------------------------------------------- //

void user_main()
{
    HAL_Delay(100); // Wait for isolated power to start up

    RelayBoard* relay = new (g_relayBuffer.data()) RelayBoard();
    relay->exec();
}

// ---------------------------------------------------------------------------------------------- //
