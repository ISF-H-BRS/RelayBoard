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

#include "maskwidget.h"
#include "ui_maskwidget.h"

#include <QFrame>
#include <QLabel>
#include <QMouseEvent>

// ---------------------------------------------------------------------------------------------- //

MaskWidget::MaskWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::MaskWidget>())
{
    m_ui->setupUi(this);

    m_bits = {
        m_ui->bit01, m_ui->bit02, m_ui->bit03, m_ui->bit04,
        m_ui->bit05, m_ui->bit06, m_ui->bit07, m_ui->bit08,
        m_ui->bit09, m_ui->bit10, m_ui->bit11, m_ui->bit12,
        m_ui->bit13, m_ui->bit14, m_ui->bit15, m_ui->bit16
    };

    m_labels = {
        m_ui->label01, m_ui->label02, m_ui->label03, m_ui->label04,
        m_ui->label05, m_ui->label06, m_ui->label07, m_ui->label08,
        m_ui->label09, m_ui->label10, m_ui->label11, m_ui->label12,
        m_ui->label13, m_ui->label14, m_ui->label15, m_ui->label16
    };
}

// ---------------------------------------------------------------------------------------------- //

MaskWidget::~MaskWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void MaskWidget::setBitSize(int size)
{
    for (QFrame* frame : m_bits)
        frame->setFixedSize(size, size);
}

// ---------------------------------------------------------------------------------------------- //

auto MaskWidget::bitSize() const -> int
{
    return m_bits.at(0)->minimumSize().width();
}

// ---------------------------------------------------------------------------------------------- //

void MaskWidget::setNumbersVisible(bool visible)
{
    for (QLabel* label : m_labels)
        label->setVisible(visible);
}

// ---------------------------------------------------------------------------------------------- //

auto MaskWidget::numbersVisible() const -> bool
{
    return m_labels.at(0)->isVisible();
}

// ---------------------------------------------------------------------------------------------- //

void MaskWidget::setEditable(bool editable)
{
    m_editable = editable;
}

// ---------------------------------------------------------------------------------------------- //

auto MaskWidget::isEditable() const -> bool
{
    return m_editable;
}

// ---------------------------------------------------------------------------------------------- //

void MaskWidget::setBitColor(size_t bit, const QColor& color)
{
    Q_ASSERT(bit < BitCount);

    QPalette palette = m_bits.at(bit)->palette();
    palette.setColor(QPalette::Active, QPalette::Base, color);
    palette.setColor(QPalette::Inactive, QPalette::Base, color);
    palette.setColor(QPalette::Active, QPalette::Window, color);
    palette.setColor(QPalette::Inactive, QPalette::Window, color);
    m_bits.at(bit)->setPalette(palette);
}

// ---------------------------------------------------------------------------------------------- //

auto MaskWidget::bitColor(size_t bit) const -> QColor
{
    Q_ASSERT(bit < BitCount);
    return m_bits.at(bit)->palette().color(QPalette::Base);
}

// ---------------------------------------------------------------------------------------------- //

void MaskWidget::setBitFilled(size_t bit, bool fill)
{
    Q_ASSERT(bit < BitCount);
    m_bits.at(bit)->setAutoFillBackground(fill);
}

// ---------------------------------------------------------------------------------------------- //

auto MaskWidget::bitFilled(size_t bit) const -> bool
{
    Q_ASSERT(bit < BitCount);
    return m_bits.at(bit)->autoFillBackground();
}

// ---------------------------------------------------------------------------------------------- //

void MaskWidget::setFillMask(uint16_t mask)
{
    for (size_t i = 0; i < BitCount; ++i)
        setBitFilled(i, mask & (1<<i));
}

// ---------------------------------------------------------------------------------------------- //

auto MaskWidget::getFillMask() const -> uint16_t
{
    uint16_t mask = 0x0000;

    for (size_t i = 0; i < BitCount; ++i)
    {
        if (bitFilled(i))
            mask |= (1<<i);
    }

    return mask;
}

// ---------------------------------------------------------------------------------------------- //

void MaskWidget::setBitEnabled(size_t bit, bool enable)
{
    Q_ASSERT(bit < BitCount);
    m_bits.at(bit)->setEnabled(enable);
}

// ---------------------------------------------------------------------------------------------- //

auto MaskWidget::bitEnabled(size_t bit) const -> bool
{
    Q_ASSERT(bit < BitCount);
    return m_bits.at(bit)->isEnabled();
}

// ---------------------------------------------------------------------------------------------- //

void MaskWidget::setEnableMask(uint16_t mask)
{
    for (size_t i = 0; i < BitCount; ++i)
        setBitEnabled(i, mask & (1<<i));
}

// ---------------------------------------------------------------------------------------------- //

auto MaskWidget::getEnableMask() const -> uint16_t
{
    uint16_t mask = 0x0000;

    for (size_t i = 0; i < BitCount; ++i)
    {
        if (bitEnabled(i))
            mask |= (1<<i);
    }

    return mask;
}

// ---------------------------------------------------------------------------------------------- //

void MaskWidget::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    if (m_editable)
    {
        const QPointF position = event->position();
        auto frame = qobject_cast<QFrame*>(childAt(position.x(), position.y()));

        if (frame && frame->isEnabled())
        {
            frame->setAutoFillBackground(!frame->autoFillBackground());
            emit maskChanged(getFillMask());
        }
    }
}

// ---------------------------------------------------------------------------------------------- //
