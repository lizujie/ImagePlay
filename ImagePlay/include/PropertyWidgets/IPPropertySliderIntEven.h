//#############################################################################
//
//  This file is part of ImagePlay.
//
//  ImagePlay is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ImagePlay is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with ImagePlay.  If not, see <http://www.gnu.org/licenses/>.
//
//#############################################################################

#ifndef IPPropertySliderIntEven_H
#define IPPropertySliderIntEven_H

#include <QWidget>
#include <QSlider>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QDebug>

#include "IPPropertyWidget.h"

class IPPropertySliderIntEven : public IPPropertyWidget
{
    Q_OBJECT
public:
    IPPropertySliderIntEven(IPLProcessPropertyInt* property, QWidget *parent) : IPPropertyWidget(property, parent)
    {
        _ignoreEvents = true;
        _lastValue = 0;

        setLayout(new QHBoxLayout);
        layout()->setMargin(0);

        _property = property;

        _slider = new QSlider(Qt::Horizontal, this);
        _spinner = new QSpinBox(this);

        layout()->addWidget(_slider);
        layout()->addWidget(_spinner);

        init();

        //connect(_slider, &QSlider::sliderReleased, this, &IPPropertySliderIntEven::updateValue );
        connect(_slider, &QSlider::valueChanged, this, &IPPropertySliderIntEven::onSliderChanged );
        connect(_spinner, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IPPropertySliderIntEven::onSpinnerChanged );

        _ignoreEvents = false;
    }

    void init()
    {
        _ignoreEvents = true;

        int min   = _property->min();
        int max   = _property->max();
        int value = _property->value();

        // slider scale is 0 - (max-min/2), step is 1
        _slider->setMinimum(0);
        _slider->setMaximum((max-min)/2);
        _slider->setValue((value-min)/2);

        // spinner scale is min - max, step is 2
        _spinner->setMinimum(min);
        _spinner->setMaximum(max);
        _spinner->setValue(value);

        _ignoreEvents = false;
    }

    void setMinimum(int v)  { _slider->setMinimum(v);  _spinner->setMinimum(v); }
    void setMaximum(int v)  { _slider->setMaximum(v);  _spinner->setMaximum(v); }
    int value()             { return _spinner->value(); init(); }

    void saveValue()        { _property->setValue(value()); }
    void resetValue()       { _property->resetValue(); }

signals:

public slots:
    void onSliderChanged(int v)
    {
        if(_ignoreEvents)
            return;
        _spinner->setValue(v*2+_spinner->minimum());
    }

    void onSpinnerChanged(int v)
    {
        // only allow even values
        if(v%2 != 0)
        {
            _ignoreEvents = true;
            if(v > _lastValue)
                _spinner->setValue(v+1);
            else
                _spinner->setValue(v-1);
            _ignoreEvents = false;
            return;
        }
        _slider->setValue((v-_spinner->minimum())/2);

        updateValue();
    }

    void updateValue()
    {
        int v = _spinner->value();

        // prevent duplicate changes
        if(v == _lastValue)
        {
            return;
        }

        if(_property)
            saveValue();

        emit changed();

        _lastValue = v;
    }

private:
    IPLProcessPropertyInt* _property;
    QSlider* _slider;
    QSpinBox* _spinner;
    int _lastValue;
    bool    _ignoreEvents;

};

#endif // IPPropertySliderIntEven_H
