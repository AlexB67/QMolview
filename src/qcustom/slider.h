#ifndef SLIDER_WIDGET_H
#define SLIDER_WIDGET_H

#include <QtWidgets/QSlider>

namespace QCustom
{
class Slider : public QSlider
{
public:
    explicit Slider(QWidget* parent = nullptr);
    ~Slider() {}

protected:
    void paintEvent(QPaintEvent *) override;
};
}

#endif
