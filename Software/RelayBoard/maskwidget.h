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

#include <QWidget>

#include <array>
#include <memory>

namespace Ui {
    class MaskWidget;
}

class QFrame;
class QLabel;

class MaskWidget : public QWidget
{
    Q_OBJECT

public:
    static constexpr size_t BitCount = 16;
    static constexpr int DefaultBitSize = 16;

public:
    explicit MaskWidget(QWidget* parent = nullptr);
    ~MaskWidget() override;

    void setBitSize(int size);
    auto bitSize() const -> int;

    void setNumbersVisible(bool visible);
    auto numbersVisible() const -> bool;

    void setEditable(bool editable);
    auto isEditable() const -> bool;

    void setBitColor(size_t bit, const QColor& color);
    auto bitColor(size_t bit) const -> QColor;

    void setBitFilled(size_t bit, bool fill);
    auto bitFilled(size_t bit) const -> bool;

    void setFillMask(uint16_t mask);
    auto getFillMask() const -> uint16_t;

    void setBitEnabled(size_t bit, bool enable);
    auto bitEnabled(size_t bit) const -> bool;

    void setEnableMask(uint16_t mask);
    auto getEnableMask() const -> uint16_t;

signals:
    void maskChanged(uint16_t mask);

private:
    void mousePressEvent(QMouseEvent* event) override;

private:
    std::unique_ptr<Ui::MaskWidget> m_ui;

    std::array<QFrame*, BitCount> m_bits;
    std::array<QLabel*, BitCount> m_labels;

    bool m_editable = false;
};
