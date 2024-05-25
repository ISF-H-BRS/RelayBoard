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
#include "limitsdialog.h"
#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QDateTime>
#include <QMessageBox>

#include <bitset>

// ---------------------------------------------------------------------------------------------- //

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_ui(std::make_unique<Ui::MainWindow>()),
      m_updateTimer(this)
{
    m_ui->setupUi(this);

    m_voltageLabels = {
        m_ui->voltage01, m_ui->voltage02, m_ui->voltage03, m_ui->voltage04,
        m_ui->voltage05, m_ui->voltage06, m_ui->voltage07, m_ui->voltage08,
        m_ui->voltage09, m_ui->voltage10, m_ui->voltage11, m_ui->voltage12,
        m_ui->voltage13, m_ui->voltage14, m_ui->voltage15, m_ui->voltage16
    };

    m_currentLabels = {
        m_ui->current01, m_ui->current02, m_ui->current03, m_ui->current04,
        m_ui->current05, m_ui->current06, m_ui->current07, m_ui->current08,
        m_ui->current09, m_ui->current10, m_ui->current11, m_ui->current12,
        m_ui->current13, m_ui->current14, m_ui->current15, m_ui->current16
    };

    auto separator = m_ui->toolBar->insertSeparator(m_ui->actionConnect);

    m_serialPortWidget = new SerialPortWidget();
    m_ui->toolBar->insertWidget(separator, m_serialPortWidget);

    m_ui->actionConnect->setEnabled(!m_serialPortWidget->selectedSerialPort().isEmpty());

    m_ui->maskSetupWidget->setBitSize(20);
    m_ui->maskSetupWidget->setEditable(true);

    for (size_t i = 0; i < MaskWidget::BitCount; ++i)
        m_ui->maskSetupWidget->setBitColor(i, Qt::green);

    m_ui->maskWidget->setBitSize(15);

    adjustSize();
    setMinimumSize(sizeHint());

    m_updateTimer.setInterval(500);
    m_updateTimer.setSingleShot(true);

    connect(m_serialPortWidget, &SerialPortWidget::serialPortChanged,
            this, &MainWindow::onSerialPortChanged);

    connect(m_ui->actionConnect, SIGNAL(triggered()), this, SLOT(openDevice()));
    connect(m_ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeDevice()));
    connect(m_ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutDialog()));

    connect(m_ui->maskSetupWidget, SIGNAL(maskChanged(uint16_t)), this, SLOT(updateMaskButtons()));

    connect(m_ui->applyButton, SIGNAL(clicked()), this, SLOT(applyMask()));
    connect(m_ui->resetButton, SIGNAL(clicked()), this, SLOT(resetMask()));
    connect(m_ui->clearButton, SIGNAL(clicked()), this, SLOT(clearMask()));

    connect(m_ui->setLimitsButton, SIGNAL(clicked()), this, SLOT(showLimitsDialog()));
    connect(m_ui->resetDeviceButton, SIGNAL(clicked()), this, SLOT(resetDevice()));

    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
}

// ---------------------------------------------------------------------------------------------- //

MainWindow::~MainWindow() = default;

// ---------------------------------------------------------------------------------------------- //

void MainWindow::onSerialPortChanged(const QString& port)
{
    m_ui->actionConnect->setEnabled(!port.isEmpty());
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::openDevice()
{
    try {
        m_device = std::make_unique<irb::Device>(m_serialPortWidget->selectedSerialPort().toUtf8());

        m_serialPortWidget->setEnabled(false);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->centralwidget->setEnabled(true);

        updateInfo();
        updateStatus();

        resetMask();
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::closeDevice()
{
    m_updateTimer.stop();
    m_device = nullptr;

    m_serialPortWidget->setEnabled(true);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->centralwidget->setEnabled(false);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::showAboutDialog()
{
    AboutDialog dialog(this);
    dialog.exec();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::updateMaskButtons()
{
    const uint16_t currentMask = m_ui->maskWidget->getFillMask();
    const uint16_t setupMask = m_ui->maskSetupWidget->getFillMask();

    m_ui->applyButton->setEnabled(setupMask != currentMask);
    m_ui->resetButton->setEnabled(setupMask != currentMask);
    m_ui->clearButton->setEnabled(setupMask != 0);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::applyMask()
{
    if (!m_device)
        return;

    try {
        m_device->setStateMask(m_ui->maskSetupWidget->getFillMask());

        updateStatus();
        updateMaskButtons();
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::resetMask()
{
    m_ui->maskSetupWidget->setFillMask(m_ui->maskWidget->getFillMask());
    updateMaskButtons();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::clearMask()
{
    m_ui->maskSetupWidget->setFillMask(0x0000);
    updateMaskButtons();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::showLimitsDialog()
{
    if (!m_device)
        return;

    try {
        LimitsDialog::LimitArray limits = {};
        bool save = false;

        for (size_t i = 0; i < irb::RelayCount; ++i)
            limits.at(i) = m_device->getPowerLimit(i);

        LimitsDialog dialog(&limits, &save, this);

        int result = dialog.exec();

        if (result == QDialog::Accepted)
        {
            for (size_t i = 0; i < irb::RelayCount; ++i)
                m_device->setPowerLimit(i, limits.at(i));

            if (save)
                m_device->savePowerLimits();
        }
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::resetDevice()
{
    if (!m_device)
        return;

    auto button = QMessageBox::question(this, "Reset Device",
                                        "Resetting the device will switch off all relays "
                                        "and clear previous error flags. Continue?");
    if (button != QMessageBox::Yes)
        return;

    try {
        m_device->reset();

        m_ui->maskSetupWidget->setFillMask(0x0000);
        m_ui->maskSetupWidget->setEnableMask(0xffff);

        updateStatus();
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::updateStatus()
{
    if (!m_device)
        return;

    try {
        const std::bitset<irb::RelayCount> faultMask = m_device->getFaultMask();
        const std::bitset<irb::RelayCount> stateMask = m_device->getStateMask();

        for (size_t i = 0; i < irb::RelayCount; ++i)
        {
            const irb::RelayPower power = m_device->getRelayPower(i);

            m_voltageLabels.at(i)->setText(QString("%1 V").arg(power.voltage, 0, 'f', 2));
            m_currentLabels.at(i)->setText(QString("%1 A").arg(power.current, 0, 'f', 3));

            if (faultMask.test(i))
            {
                m_ui->maskWidget->setBitColor(i, Qt::red);
                m_ui->maskWidget->setBitFilled(i, true);

                m_ui->maskSetupWidget->setBitFilled(i, false);
                m_ui->maskSetupWidget->setBitEnabled(i, false);
            }
            else if (stateMask.test(i))
            {
                m_ui->maskWidget->setBitColor(i, Qt::black);
                m_ui->maskWidget->setBitFilled(i, true);
            }
            else
                m_ui->maskWidget->setBitFilled(i, false);
        }

        m_updateTimer.start();
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::updateInfo()
{
    if (!m_device)
        return;

    m_ui->hardwareVersion->setText(m_device->getHardwareVersion().c_str());
    m_ui->firmwareVersion->setText(m_device->getFirmwareVersion().c_str());
    m_ui->serialNumber->setText(m_device->getSerialNumber().c_str());

    const auto date = QDateTime::fromSecsSinceEpoch(m_device->getBuildTimestamp()).date();
    m_ui->buildDate->setText(QLocale::system().toString(date, QLocale::ShortFormat));
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handleError(const QString& msg)
{
    closeDevice();
    QMessageBox::critical(this, "Error", msg);
}

// ---------------------------------------------------------------------------------------------- //
