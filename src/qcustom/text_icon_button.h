#ifndef TEXTICON_BUTTON_H
#define TEXTICON_BUTTON_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>

namespace QCustom
{
class TextIconButton : public QPushButton
{
public:
    explicit TextIconButton(const QString& text, const QIcon& icon, QWidget* parent = nullptr);
    ~TextIconButton() {}

    void setButtonText(const QString& text);
    void setButtonIcon(const QIcon& icon, bool flipX = false, bool flipY = false);
    void mirrorIcon(const bool flipX, const bool flipY);

    TextIconButton(const TextIconButton &) = delete;
    TextIconButton &operator=(const TextIconButton &other) = delete;
    TextIconButton(const TextIconButton &&) = delete;
    TextIconButton &&operator=(const TextIconButton &&other) = delete;

    void setText() = delete;
    void setIcon() = delete;
    QString text() = delete;
    QIcon icon() = delete;

    QSize sizeHint() const override;

private:
    QString m_text;
    QIcon m_icon;
    QPixmap pm;
    QPixmap pm2;

protected:
    void paintEvent(QPaintEvent* e) override;
};
}

#endif
