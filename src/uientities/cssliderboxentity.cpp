/*
 *  Cascade Image Editor
 *
 *  Copyright (C) 2022 Till Dechent and contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "cssliderboxentity.h"

#include <QMouseEvent>

#include "../nodegraph/nodeproperties.h"

namespace Cascade {

CsSliderBoxEntity::CsSliderBoxEntity(
        UIElementType et,
        QWidget *parent) :
    UiEntity(et, parent),
    mUi(new Ui::CsSliderBox)
{
    mUi->setupUi(this);

    mNameLabel = new QLabel(this);
    mNameLabel->setText("None");
    mNameLabel->setObjectName("nameLabel");
    mUi->gridLayout->addWidget(mNameLabel, 0, 0, Qt::AlignLeft | Qt::AlignVCenter);

    if (mElementType == UIElementType::eSliderBoxDouble)
    {
        mValueBoxDouble = new QDoubleSpinBox(this);
        mValueBoxDouble->setButtonSymbols(QAbstractSpinBox::NoButtons);
        mUi->gridLayout->addWidget(mValueBoxDouble, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);
        mValueBoxDouble->installEventFilter(this);
        mIsDouble = true;
    }
    else
    {
        mValueBoxInt = new QSpinBox(this);
        mValueBoxInt->setButtonSymbols(QAbstractSpinBox::NoButtons);
        mUi->gridLayout->addWidget(mValueBoxInt, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);
        mValueBoxInt->installEventFilter(this);
    }

    mUi->slider->installEventFilter(this);
    mNameLabel->installEventFilter(this);

    connect(mUi->slider, &SliderNoClick::valueChanged,
            this, &CsSliderBoxEntity::handleSliderValueChanged);

    if (mElementType == UIElementType::eSliderBoxDouble)
        connect(mValueBoxDouble, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &CsSliderBoxEntity::handleSpinBoxValueChanged);
    else
        connect(mValueBoxInt, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &CsSliderBoxEntity::handleSpinBoxValueChanged);
}

void CsSliderBoxEntity::selfConnectToValueChanged(NodeProperties* p)
{
    connect(this, &CsSliderBoxEntity::valueChanged,
            p, [p]{p->handleSomeValueChanged();});
}

const QString CsSliderBoxEntity::name()
{
    return mNameLabel->text();
}

void CsSliderBoxEntity::setName(const QString &name)
{
    mNameLabel->setText(name);
}

QString CsSliderBoxEntity::getValuesAsString()
{
    if (mElementType == UIElementType::eSliderBoxDouble)
        return QString::number(mUi->slider->value() / DOUBLE_MULT);
    else
        return QString::number(mUi->slider->value());
}

void CsSliderBoxEntity::loadPropertyValues(const QString &values)
{
    if (mIsDouble)
        mValueBoxDouble->setValue(values.toFloat());
    else
        mValueBoxInt->setValue(values.toInt());
}

void CsSliderBoxEntity::handleSliderValueChanged()
{
    setSpinBoxNoSignal(mUi->slider->value());
}

void CsSliderBoxEntity::handleSpinBoxValueChanged()
{
    if (mElementType == UIElementType::eSliderBoxDouble)
        setSliderNoSignal(mValueBoxDouble->value());
    else
        setSliderNoSignal(mValueBoxInt->value());
}

void CsSliderBoxEntity::reset()
{
    if (mElementType == UIElementType::eSliderBoxDouble)
        mValueBoxDouble->setValue(mBaseValue);
    else
        mValueBoxInt->setValue(mBaseValue);
}

bool CsSliderBoxEntity::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if(mouseEvent->button() == Qt::LeftButton)
        {
            mLastPos = QCursor::pos();
            mIsDragging = true;

            if (QApplication::queryKeyboardModifiers() == Qt::ControlModifier)
            {
                reset();
            }
        }
    }
    if(event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if(mouseEvent->button() == Qt::LeftButton)
        {
            mIsDragging = false;
        }
    }

    Q_UNUSED(watched);

    return false;
}

void CsSliderBoxEntity::mouseMoveEvent(QMouseEvent* event)
{
    if(mIsDragging)
    {
        float dx = QCursor::pos().x() - mLastPos.x();
        int lastValue = mUi->slider->value();
        float offset = dx * (mUi->slider->singleStep() * ((mUi->slider->maximum() - mUi->slider->minimum()) / 200.0));
        if (offset > 0.0 && offset  < 1.0)
            offset = 1.0;
        else if (offset > -1.0 && offset < 0.0)
            offset = -1.0;
        int newValue = lastValue + offset;

        mUi->slider->setValue(newValue);
    }
    mLastPos = QCursor::pos();

    Q_UNUSED(event);
}

CsSliderBoxEntity::~CsSliderBoxEntity()
{
    delete mUi;
}

} // namespace Cascade
