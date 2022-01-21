#include "colour_button.h"
#include <QPainter>
#include <QPixmap>

QCustom::ColourButton::ColourButton(QWidget* parent)
: QPushButton(parent)
{
}

void QCustom::ColourButton::setColour(const QColor& colour)
{
    m_Colour = colour;
    update(contentsRect());
    Q_EMIT colourChanged();
}

void QCustom::ColourButton::paintEvent(QPaintEvent* event)
{
    QPushButton::paintEvent(event);

    QColor tmp = (isActiveWindow() && isEnabled()) ? m_Colour : QColor(Qt::gray);

    QPainter p(this);
    QPainter::RenderHints m_paintFlags = QPainter::RenderHints(QPainter::Antialiasing);
    p.setRenderHints(m_paintFlags, true);
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(tmp));
    p.drawRect(6, 6, contentsRect().width() - 12, contentsRect().height() - 12);
}
