#include "slider.h"
#include <cmath>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QStyleFactory>
#include <QStylePainter>
#include <QStyle>
#include <QStyleOption>

QCustom::Slider::Slider(QWidget* parent)
: QSlider(Qt::Orientation::Horizontal, parent)
{
}

void QCustom::Slider::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    QRect handle = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    int interval = tickInterval();
    if (interval == 0)
    {
        interval = pageStep();
    }

    p.save();
    // Custom ticks to solve some themes setting black ticks on a dark theme
    // So override
    if (tickPosition() != NoTicks)
    {
        QColor col = qApp->palette().brush(QPalette::Text).color();

        for (int i = minimum(); i <= maximum(); i += interval)
        {
            int x = static_cast<int>(std::round(static_cast<double>(static_cast<double>(static_cast<double>(i - minimum())
            / static_cast<double>(maximum() - minimum())) * static_cast<double>(width() - handle.width())
            + static_cast<double>(static_cast<double>(handle.width()) / 2.0))) - 1);
            int h = 4;
            p.setPen(col);
            p.setOpacity(0.6);

            if (tickPosition() == TicksBothSides || tickPosition() == TicksAbove)
            {
                int y = rect().top();
                p.drawLine(x, y, x, y + h);
            }

            if (tickPosition() == TicksBothSides || tickPosition() == TicksBelow)
            {
                int y = rect().bottom();
                p.drawLine(x, y, x, y - h);
            }

        }
    }

    p.restore();

    opt.subControls = QStyle::SC_SliderGroove;
    p.drawComplexControl(QStyle::CC_Slider, opt);
    opt.subControls = QStyle::SC_SliderHandle;
    p.drawComplexControl(QStyle::CC_Slider, opt);
}
