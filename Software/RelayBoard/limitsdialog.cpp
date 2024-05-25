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

#include "limitsdialog.h"
#include "ui_limitsdialog.h"

#include <QFileDialog>
#include <QSettings>

// ---------------------------------------------------------------------------------------------- //

LimitsDialog::LimitsDialog(LimitArray* powers, bool* save, QWidget* parent)
    : QDialog(parent),
      m_ui(std::make_unique<Ui::LimitsDialog>()),
      m_powers(powers),
      m_save(save)
{
    Q_ASSERT(powers != nullptr);
    Q_ASSERT(save != nullptr);

    m_ui->setupUi(this);

    m_voltageSpinBoxes = {
        m_ui->voltage1,  m_ui->voltage2,  m_ui->voltage3,  m_ui->voltage4,
        m_ui->voltage5,  m_ui->voltage6,  m_ui->voltage7,  m_ui->voltage8,
        m_ui->voltage9,  m_ui->voltage10, m_ui->voltage11, m_ui->voltage12,
        m_ui->voltage13, m_ui->voltage14, m_ui->voltage15, m_ui->voltage16
    };

    m_currentSpinBoxes = {
        m_ui->current1,  m_ui->current2,  m_ui->current3,  m_ui->current4,
        m_ui->current5,  m_ui->current6,  m_ui->current7,  m_ui->current8,
        m_ui->current9,  m_ui->current10, m_ui->current11, m_ui->current12,
        m_ui->current13, m_ui->current14, m_ui->current15, m_ui->current16
    };

    auto applyButton = m_ui->buttonBox->button(QDialogButtonBox::Apply);
    auto saveButton = m_ui->buttonBox->button(QDialogButtonBox::Save);
    auto resetButton = m_ui->buttonBox->button(QDialogButtonBox::Reset);
    auto restoreButton = m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults);

    applyButton->setWhatsThis("Applies power limits to the device temporarily.");
    applyButton->setToolTip(applyButton->whatsThis());

    saveButton->setText("Save to Device");
    saveButton->setWhatsThis("Saves power limits to persistent memory on the device.");
    saveButton->setToolTip(saveButton->whatsThis());

    resetButton->setWhatsThis("Resets fields to the values currently used by the device.");
    resetButton->setToolTip(resetButton->whatsThis());
    resetButton->setEnabled(false);

    restoreButton->setWhatsThis("Resets all fields to their default values.");
    restoreButton->setToolTip(restoreButton->whatsThis());

    m_ui->saveProfileButton->setWhatsThis("Saves the chosen power limits to a local file.");
    m_ui->saveProfileButton->setToolTip(m_ui->saveProfileButton->whatsThis());

    m_ui->loadProfileButton->setWhatsThis("Loads saved power limits from a local file.");
    m_ui->loadProfileButton->setToolTip(m_ui->loadProfileButton->whatsThis());

    setFixedSize(minimumSizeHint());

    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(restoreButton, SIGNAL(clicked()), this, SLOT(restoreDefaults()));

    connect(m_ui->saveProfileButton, SIGNAL(clicked()), this, SLOT(saveProfile()));
    connect(m_ui->loadProfileButton, SIGNAL(clicked()), this, SLOT(loadProfile()));

    for (size_t i = 0; i < irb::RelayCount; ++i)
    {
        connect(m_voltageSpinBoxes.at(i), SIGNAL(valueChanged(double)),
                this, SLOT(updateResetButton()));
        connect(m_currentSpinBoxes.at(i), SIGNAL(valueChanged(double)),
                this, SLOT(updateResetButton()));
    }

    reset();
}

// ---------------------------------------------------------------------------------------------- //

LimitsDialog::~LimitsDialog() = default;

// ---------------------------------------------------------------------------------------------- //

void LimitsDialog::updateResetButton()
{
    const auto changed = [this]
    {
        for (size_t i = 0; i < irb::RelayCount; ++i)
        {
            const irb::RelayPower power = m_powers->at(i);

            if (m_voltageSpinBoxes.at(i)->value() != power.voltage)
                return true;

            if (m_currentSpinBoxes.at(i)->value() != power.current)
                return true;
        }

        return false;
    };

    m_ui->buttonBox->button(QDialogButtonBox::Reset)->setEnabled(changed());
}

// ---------------------------------------------------------------------------------------------- //

void LimitsDialog::apply()
{
    storeLimits();
    *m_save = false;
    accept();
}

// ---------------------------------------------------------------------------------------------- //

void LimitsDialog::save()
{
    storeLimits();
    *m_save = true;
}

// ---------------------------------------------------------------------------------------------- //

void LimitsDialog::reset()
{
    for (size_t i = 0; i < irb::RelayCount; ++i)
    {
        const irb::RelayPower power = m_powers->at(i);

        m_voltageSpinBoxes.at(i)->setValue(power.voltage);
        m_currentSpinBoxes.at(i)->setValue(power.current);
    }
}

// ---------------------------------------------------------------------------------------------- //

void LimitsDialog::restoreDefaults()
{
    for (size_t i = 0; i < irb::RelayCount; ++i)
    {
        m_voltageSpinBoxes.at(i)->setValue(irb::MaximumVoltageLimit);
        m_currentSpinBoxes.at(i)->setValue(irb::MaximumCurrentLimit);
    }
}

// ---------------------------------------------------------------------------------------------- //

void LimitsDialog::saveProfile()
{
    const QString fileName =
            QFileDialog::getSaveFileName(this, "Save Profile", QString(), "Profiles (*.prof)");

    if (fileName.isEmpty())
        return;

    QSettings settings(fileName, QSettings::IniFormat);
    settings.beginGroup("Profile");

    for (size_t i = 0; i < irb::RelayCount; ++i)
    {
        settings.setValue(makeVoltageKey(i), m_voltageSpinBoxes.at(i)->value());
        settings.setValue(makeCurrentKey(i), m_currentSpinBoxes.at(i)->value());
    }
}

// ---------------------------------------------------------------------------------------------- //

void LimitsDialog::loadProfile()
{
    const QString fileName =
            QFileDialog::getOpenFileName(this, "Load Profile", QString(), "Profiles (*.prof)");

    if (fileName.isEmpty())
        return;

    QSettings settings(fileName, QSettings::IniFormat);
    settings.beginGroup("Profile");

    for (size_t i = 0; i < irb::RelayCount; ++i)
    {
        double voltage = m_voltageSpinBoxes.at(i)->value();
        voltage = settings.value(makeVoltageKey(i), voltage).toDouble();
        m_voltageSpinBoxes.at(i)->setValue(settings.value(makeVoltageKey(i), voltage).toDouble());

        double current = m_currentSpinBoxes.at(i)->value();
        current = settings.value(makeCurrentKey(i), current).toDouble();
        m_currentSpinBoxes.at(i)->setValue(current);
    }
}

// ---------------------------------------------------------------------------------------------- //

void LimitsDialog::storeLimits()
{
    for (size_t i = 0; i < irb::RelayCount; ++i)
        m_powers->at(i) = { m_voltageSpinBoxes.at(i)->value(), m_currentSpinBoxes.at(i)->value() };
}

// ---------------------------------------------------------------------------------------------- //

auto LimitsDialog::makeVoltageKey(size_t index) -> QString
{
    return QString("Voltage%1").arg(index, 2, 10, QChar('0'));
}

// ---------------------------------------------------------------------------------------------- //

auto LimitsDialog::makeCurrentKey(size_t index) -> QString
{
    return QString("Current%1").arg(index, 2, 10, QChar('0'));
}

// ---------------------------------------------------------------------------------------------- //
