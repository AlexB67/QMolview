#ifndef COLOUR_BUTTON_H
#define COLOUR_BUTTON_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

namespace QCustom
{
class ColourButton : public QPushButton
{
    Q_OBJECT

public:
    explicit  ColourButton(QWidget* parent = nullptr);
    ~ ColourButton() {}

    void setColour(const QColor& colour);
    const QColor& Colour() const { return m_Colour;}

    ColourButton(const ColourButton &) = delete;
    ColourButton &operator=(const ColourButton &other) = delete;
    ColourButton(const ColourButton &&) = delete;
    ColourButton &&operator=(const ColourButton &&other) = delete;

    void setText() = delete;
    void setIcon() = delete;
    QString text() = delete;
    QIcon icon() = delete;

Q_SIGNALS:
    void colourChanged();

private:
    QColor       m_Colour{Qt::gray};
    QPixmap      pm;

protected:
    void paintEvent(QPaintEvent* e) override;
};
}

#endif
