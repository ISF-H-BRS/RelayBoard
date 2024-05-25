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

QT += core gui serialport widgets

TARGET = FirmwareUpdater
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG(release, debug|release): DEFINES += NDEBUG

CONFIG += c++2a

unix {
    QMAKE_RPATHDIR = $ORIGIN
}

FIRMWAREUPDATER_ROOT = $$(FIRMWAREUPDATER_ROOT)

isEmpty(FIRMWAREUPDATER_ROOT) {
    message(FIRMWAREUPDATER_ROOT is not set. Compilation will likely fail.)
}

INCLUDEPATH += $$FIRMWAREUPDATER_ROOT/include
INCLUDEPATH += ../libIRB/include

LIBS += -L. -lFirmwareUpdaterCore -lFirmwareUpdaterUi -lIRB

SOURCES += \
    aboutdialog.cpp \
    component.cpp \
    componentfactory.cpp \
    main.cpp \
    mainwindow.cpp \
    serialportwidget.cpp

HEADERS += \
    aboutdialog.h \
    component.h \
    componentfactory.h \
    mainwindow.h \
    serialportwidget.h

FORMS += \
    aboutdialog.ui \
    serialportwidget.ui

RESOURCES += \
    FirmwareUpdater.qrc
