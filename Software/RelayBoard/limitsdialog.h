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

#include <irb.h>

#include <QDialog>
#include <QDoubleSpinBox>

#include <array>
#include <memory>

namespace Ui {
    class LimitsDialog;
}

class LimitsDialog : public QDialog
{
    Q_OBJECT

public:
    using LimitArray = std::array<irb::RelayPower, irb::RelayCount>;

public:
    explicit LimitsDialog(LimitArray* powers, bool* save, QWidget* parent = nullptr);
    ~LimitsDialog() override;

private slots:
    void updateResetButton();

    void apply();
    void save();
    void reset();
    void restoreDefaults();

    void saveProfile();
    void loadProfile();

private:
    void storeLimits();

    static auto makeVoltageKey(size_t index) -> QString;
    static auto makeCurrentKey(size_t index) -> QString;

private:
    std::unique_ptr<Ui::LimitsDialog> m_ui;

    std::array<QDoubleSpinBox*, irb::RelayCount> m_voltageSpinBoxes;
    std::array<QDoubleSpinBox*, irb::RelayCount> m_currentSpinBoxes;

    LimitArray* m_powers;
    bool* m_save;
};
