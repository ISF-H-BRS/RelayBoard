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

TARGET = RelayBoard
TEMPLATE = app

CONFIG += c++2a

INCLUDEPATH += ../libIRB/include
LIBS += -L. -lIRB

QMAKE_RPATHDIR += $ORIGIN

SOURCES += \
    aboutdialog.cpp \
    limitsdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    maskwidget.cpp \
    serialportwidget.cpp

HEADERS += \
    aboutdialog.h \
    limitsdialog.h \
    mainwindow.h \
    maskwidget.h \
    serialportwidget.h

FORMS += \
    aboutdialog.ui \
    limitsdialog.ui \
    mainwindow.ui \
    maskwidget.ui \
    serialportwidget.ui

RESOURCES += \
    RelayBoard.qrc
