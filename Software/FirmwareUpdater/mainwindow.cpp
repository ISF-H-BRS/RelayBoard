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

#include "aboutdialog.h"
#include "device.h"
#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QProgressDialog>

#include <thread>

// ---------------------------------------------------------------------------------------------- //

MainWindow::MainWindow(QWidget* parent)
    : FirmwareUpdater::MainWindow(parent)
{
    setWindowTitle("IRB Firmware Updater");

    m_actionConnect = toolBar()->addAction(QIcon(":/res/connect.svg"), "Connect");
    m_actionDisconnect = toolBar()->addAction(QIcon(":/res/disconnect.svg"), "Disconnect");
    m_actionAbout = toolBar()->addAction(QIcon(":/res/about.svg"), "About");

    auto separator = toolBar()->insertSeparator(m_actionConnect);

    m_serialPortWidget = new SerialPortWidget();
    toolBar()->insertWidget(separator, m_serialPortWidget);

    updateUi();

    connect (m_serialPortWidget, &SerialPortWidget::serialPortChanged, this, &MainWindow::updateUi);

    connect(m_actionConnect, &QAction::triggered, this, &MainWindow::openDevice);
    connect(m_actionDisconnect, &QAction::triggered, this, &MainWindow::closeDevice);
    connect(m_actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    appendLog("Client started.");
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::updateUi()
{
    const bool connected = (m_componentFactory != nullptr);

    m_serialPortWidget->setEnabled(!connected);

    m_actionConnect->setEnabled(!connected && !m_serialPortWidget->selectedSerialPort().isEmpty());
    m_actionDisconnect->setEnabled(connected);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::openDevice()
{
    Q_ASSERT(m_componentFactory == nullptr);

    const QString port = m_serialPortWidget->selectedSerialPort();
    Q_ASSERT(!port.isEmpty());

    try {
        auto device = std::make_unique<irb::Private::Device>(port.toLocal8Bit());
        m_componentFactory = std::make_unique<ComponentFactory>(std::move(device));

        updateUi();
        appendLog("Connected to device.");

        setConnected(m_componentFactory.get());
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::closeDevice()
{
    if (!m_componentFactory)
        return;

    setDisconnected();

    m_componentFactory = nullptr;

    updateUi();
    appendLog("Connection to device closed.");
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::showAboutDialog()
{
    AboutDialog dialog(this);
    dialog.exec();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handleError(const QString& msg)
{
    FirmwareUpdater::MainWindow::handleError(msg);
    closeDevice();
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchBootloader(int uniqueId) -> bool
{
    assert(uniqueId == ComponentFactory::DeviceUniqueId);

    const QString msg = "The device will now be reset to bootloader mode. "
                        "Please wait for the hardware to reinitialize, then reconnect.";

    const auto button =
            QMessageBox::information(this, "Note", msg, QMessageBox::Ok | QMessageBox::Cancel);

    return button == QMessageBox::Ok;
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::postLaunchBootloader(int uniqueId)
{
    assert(uniqueId == ComponentFactory::DeviceUniqueId);
    closeDevice();
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::confirmLaunchFirmware(int uniqueId) -> bool
{
    assert(uniqueId == ComponentFactory::DeviceUniqueId);

    const QString msg = "The device will now be reset to firmware mode. "
                        "Please wait for the hardware to reinitialize, then reconnect.";

    const auto button =
            QMessageBox::information(this, "Note", msg, QMessageBox::Ok | QMessageBox::Cancel);

    return button == QMessageBox::Ok;
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::postLaunchFirmware(int uniqueId)
{
    assert(uniqueId == ComponentFactory::DeviceUniqueId);
    closeDevice();
}

// ---------------------------------------------------------------------------------------------- //
