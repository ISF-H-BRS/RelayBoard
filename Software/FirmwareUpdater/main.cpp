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

#include "mainwindow.h"

#include <FirmwareUpdater/core.h>

#include <QApplication>

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr const char* PublicKey =
            "-----BEGIN RSA PUBLIC KEY-----\n"
            "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAwEMfzV5lWIkCOys5d5tU\n"
            "qf7p21PRQEYbnKZPauPe+bzDIesWfauqKpYBm85Mx7Kb3p3IXXKJDgUu/gZxX6zw\n"
            "wr7VQUS4y4GDmsZWmkI54MVGw56l5NXONVPo8j//XTiQKpjalUIq2MYr/sqzW9lR\n"
            "FzTdEs7T6YQqFfGAhiUMuf/5y+4gV7kkE3KSmPz6WR0XNCRr4EALvSeo2jiAmi9d\n"
            "HooQJ9R9gj1fCDyIIuPz6SPdFEnwXIl3VEpP01zUJ5Wd5tIEuD9Nq9ZW2QqgC1pW\n"
            "0SXNvKqq+viMfxalW0Vm2cgDa202NTvx1c9CXKpPN5u3Y1GarMAsxRraaENaMWfQ\n"
            "ITxCp++pMWdC+cwV3U5V0XCaZvcaNe1LZsRP7zr7BV5Ori+Bx6LG2hHO1xwUSkgB\n"
            "IQwWwXWm46mXLs+F4nBCiwu5KFS/sGlgB00SBg3RtL/OsExdl3ocTQnvgsScq3hv\n"
            "aZ8DleH2Cq7ibW3qyMPOtlry7S6SMQ3vaa6ARX0Uiok4+KgPIE3i97JwD8fzcIHV\n"
            "LBH0BRxY26kBVJdQP7VC79fSgkQNOEgXEpORlHqZnq0v0GzJnpk5hQZFZ7CzmlRO\n"
            "YmKZhB/f+veEoS9fqO5V91nJbXN+mKIAnp0SEHS+XEPWlztJIDaRnqebnNkOof5M\n"
            "25xtUa27iYQUaLGh6N4XBwkCAwEAAQ==\n"
            "-----END RSA PUBLIC KEY-----\n";
}

// ---------------------------------------------------------------------------------------------- //

auto main(int argc, char* argv[]) -> int
{
    FirmwareUpdater::FirmwareArchive::registerPublicKey("ISF", PublicKey);

    QCoreApplication::setOrganizationName("Bonn-Rhein-Sieg University of Applied Sciences");
    QCoreApplication::setApplicationName("IRB Firmware Updater");
    QCoreApplication::setApplicationVersion("1.0");

    QApplication application(argc, argv);

    MainWindow window;
    window.show();

    return QApplication::exec();
}

// ---------------------------------------------------------------------------------------------- //
