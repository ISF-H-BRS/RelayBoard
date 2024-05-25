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

#include "serialportwidget.h"

#include <irb.h>

#include <QLabel>
#include <QMainWindow>
#include <QTimer>

#include <memory>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSerialPortChanged(const QString& port);

    void openDevice();
    void closeDevice();

    void showAboutDialog();

    void updateMaskButtons();

    void applyMask();
    void resetMask();
    void clearMask();

    void showLimitsDialog();
    void resetDevice();

    void updateStatus();

private:
    void updateInfo();
    void handleError(const QString& msg);

private:
    std::unique_ptr<Ui::MainWindow> m_ui;
    SerialPortWidget* m_serialPortWidget = nullptr;

    std::array<QLabel*, irb::RelayCount> m_voltageLabels = {};
    std::array<QLabel*, irb::RelayCount> m_currentLabels = {};

    std::unique_ptr<irb::Device> m_device;

    QTimer m_updateTimer;
};
