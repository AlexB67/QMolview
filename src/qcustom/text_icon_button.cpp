#include "text_icon_button.h"
#include <QPainter>


QCustom::TextIconButton::TextIconButton(const QString& text, const QIcon& icon, QWidget* parent) 
: QPushButton(parent), m_text(text), m_icon(icon)
{
    setButtonIcon(icon);
    update(rect());
}

void QCustom::TextIconButton::setButtonText(const QString& text) 
{ 
    m_text = text;
    update(rect());
}

void QCustom::TextIconButton::mirrorIcon(const bool flipX, const bool flipY)
{
    pm = m_icon.pixmap(16, 16);
    QImage img = m_icon.pixmap(QSize(16, 16)).toImage().convertToFormat(QImage::Format_ARGB32).mirrored(flipX, flipY);
    if (flipX || flipY) pm = QPixmap::fromImage(img);

    QPainter p_img(&img);
    p_img.save();
    p_img.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    p_img.fillRect(img.rect(), QColor(qRgba(160, 160, 160, 0)));
    p_img.restore();

    pm2 = QPixmap::fromImage(img); // disabled icon

    update(rect());
}

void QCustom::TextIconButton::setButtonIcon(const QIcon& icon, bool flipX, bool flipY)
{
    m_icon = icon;
    mirrorIcon(flipX, flipY);
}

void QCustom::TextIconButton::paintEvent(QPaintEvent* event)
{
    QPushButton::paintEvent(event);

    QPainter p(this);
    QPainter::RenderHints m_paintFlags = QPainter::RenderHints(QPainter::Antialiasing);
    p.setRenderHints(m_paintFlags, true);

    if (isEnabled())
    {
        const int yoffset = (height() - pm.height()) / 2;
        p.translate(contentsRect().width() / 4 - pm.width() / 2,  0);
        p.drawPixmap(0, yoffset, pm);
        p.translate(-contentsRect().width() / 4 + pm.width() / 2,  0);
    }
    else
    {
        const int yoffset = (height() - pm2.height()) / 2;
        p.translate(contentsRect().width() / 4 - pm2.width() / 2,  0);
        p.drawPixmap(0, yoffset, pm2);
        p.translate(-contentsRect().width() / 4 + pm2.width() / 2,  0);
    }

    QFontMetrics fm(p.font());
    p.translate(3 * contentsRect().width() / 4 - fm.boundingRect(m_text).width() / 2, 0);
    p.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
}

QSize QCustom::TextIconButton::sizeHint() const
{
    const auto button_hint = QPushButton::sizeHint();
    return QSize(button_hint.width() + 2 * pm.width(), std::max(button_hint.height(), button_hint.height()));
}
