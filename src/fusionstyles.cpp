#include "fusionstyles.h"
#include <QIcon>
#include <QPixmap>
#include <iostream>

void FusionStyle::FusionDarkStyle::polish(QPalette &palette)
{
    QColor fusion_colour = QColor(53,53,53);
    QColor fusion_disabled_colour = QColor(Qt::gray);
    palette.setColor(QPalette::Window, fusion_colour);
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(37, 37, 37));
    palette.setColor(QPalette::AlternateBase, fusion_colour);
    palette.setColor(QPalette::ToolTipBase, Qt::black);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, fusion_disabled_colour);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, fusion_disabled_colour);
    palette.setColor(QPalette::Button, fusion_colour);
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, fusion_disabled_colour);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::LinkVisited, QColor(42, 130, 218).darker());
    palette.setColor(QPalette::Highlight, Qt::lightGray);
    palette.setColor(QPalette::HighlightedText, Qt::lightGray);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, fusion_disabled_colour);
}

void FusionStyle::FusionLightStyle::polish(QPalette &palette)
{
    QColor fusion_colour = QColor(194, 225, 237);
    QColor fusion_disabled_colour = QColor(Qt::gray);
    palette.setColor(QPalette::Window, fusion_colour);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Base, QColor(228, 243, 249));
    palette.setColor(QPalette::AlternateBase, fusion_colour);
    palette.setColor(QPalette::ToolTipBase, fusion_colour.lighter());
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::Text, fusion_disabled_colour);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, fusion_disabled_colour);
    palette.setColor(QPalette::Button, fusion_colour);
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, fusion_disabled_colour);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(2, 20, 188));
    palette.setColor(QPalette::LinkVisited, fusion_disabled_colour.darker());
    palette.setColor(QPalette::Highlight, QColor(60, 140, 230));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, fusion_disabled_colour);
}

void FusionStyle::FusionZodiacBlueStyle::polish(QPalette &palette)
{
    QColor fusion_colour = QColor(61, 66, 80);
    QColor fusion_disabled_colour = QColor(Qt::gray);
    palette.setColor(QPalette::Window, fusion_colour);
    palette.setColor(QPalette::WindowText, QColor(240, 240, 240));
    palette.setColor(QPalette::Base, QColor(41, 46, 60));
    palette.setColor(QPalette::AlternateBase, fusion_colour);
    palette.setColor(QPalette::ToolTipBase, Qt::blue);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, fusion_disabled_colour);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, fusion_disabled_colour);
    palette.setColor(QPalette::Button, fusion_colour);
    palette.setColor(QPalette::ButtonText,QColor(240, 240, 240));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, fusion_disabled_colour);
    palette.setColor(QPalette::BrightText, QColor(Qt::white));
    palette.setColor(QPalette::Link, QColor(42, 180, 248));
    palette.setColor(QPalette::LinkVisited, QColor(42, 130, 218).darker());
    palette.setColor(QPalette::Highlight, QColor(62, 150, 238));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, fusion_disabled_colour);
}

// Zodiac

int FusionStyle::FusionZodiacBlueStyle::styleHint(StyleHint hint, const QStyleOption *option,
                                                  const QWidget *widget,
                                                  QStyleHintReturn *returnData) const
{
    switch (hint)
    {
        case SH_DitherDisabledText:
            return int(false);
        case SH_EtchDisabledText:
            return int(true);
        case SH_GroupBox_TextLabelColor:
            return int(true);
        default:
            return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
}

QPixmap FusionStyle::FusionZodiacBlueStyle::generatedIconPixmap(
                          QIcon::Mode iconMode, const QPixmap &pixmap,
                          const QStyleOption *option) const
{
    if (iconMode == QIcon::Disabled)
    {
        QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
        QPainter painter(&img);
        painter.save();
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(img.rect(), QColor(qRgba(160, 160, 160, 0)));
        painter.restore();
        return QPixmap::fromImage(img);
    }

    return QProxyStyle::generatedIconPixmap(iconMode, pixmap, option);
}

void FusionStyle::FusionZodiacBlueStyle::drawControl(ControlElement element,
                                                     const QStyleOption *option,
                                                     QPainter *painter,
                                                     const QWidget *widget) const
{
    switch (element)
    {
    case CE_PushButtonLabel:
        {
            QStyleOptionButton myButtonOption;
            const QStyleOptionButton *buttonOption =
                    qstyleoption_cast<const QStyleOptionButton *>(option);
            if (buttonOption) {
                myButtonOption = *buttonOption;
                if (myButtonOption.palette.currentColorGroup()
                        != QPalette::Disabled) {
                    if (myButtonOption.state & (State_Sunken | State_On)) {
                        myButtonOption.palette.setBrush(QPalette::ButtonText,
                                myButtonOption.palette.brightText());
                    }
                }
            }

            QProxyStyle::drawControl(element, &myButtonOption, painter, widget);
        }
        break;
    default:
        QProxyStyle::drawControl(element, option, painter, widget);
    }
}

// Dark

void FusionStyle::FusionDarkStyle::polish(QWidget *widget)
{
    if (!widget)
    {
         return;
    }

    if (qobject_cast<QAbstractItemView *>(widget)
            || qobject_cast<QAbstractSpinBox *>(widget)
            || qobject_cast<QCheckBox *>(widget)
            || qobject_cast<QComboBox *>(widget)
            || qobject_cast<QDial *>(widget)
            || qobject_cast<QLineEdit *>(widget)
            || qobject_cast<QPushButton *>(widget)
            || qobject_cast<QRadioButton *>(widget)
            || qobject_cast<QScrollBar *>(widget)
            || qobject_cast<QSlider *>(widget)
            || qobject_cast<QSplitterHandle *>(widget)
            || qobject_cast<QTabBar *>(widget)
            || qobject_cast<QTextEdit *>(widget)
            || qobject_cast<QToolButton *>(widget)
            || qobject_cast<QHeaderView *>(widget)
            || widget->inherits("KTextEditor::View"))
    {
        widget->setAttribute(Qt::WA_Hover);
    }

    if (qobject_cast<QTabBar *>(widget))
    {
        qobject_cast<QTabBar *>(widget)->setDrawBase(true);
    }
//    if (qobject_cast<QPushButton *>(widget)
//            || qobject_cast<QComboBox *>(widget))
//    {
//        widget->setAttribute(Qt::WA_Hover, true);
//    }
}

void FusionStyle::FusionDarkStyle::unpolish(QWidget *widget)
{
    if (qobject_cast<QAbstractScrollArea *>(widget)
            || qobject_cast<QDockWidget *>(widget)
            || qobject_cast<QMdiSubWindow *>(widget)
            || widget->inherits("QComboBoxPrivateContainer")
            || qobject_cast<QDialog *>(widget)
            || qobject_cast<QMainWindow *>(widget))
    {
        widget->removeEventFilter(this);
    }

    QProxyStyle::unpolish(widget);

//    if (qobject_cast<QPushButton *>(widget)
//            || qobject_cast<QComboBox *>(widget))
//    {
//        widget->setAttribute(Qt::WA_Hover, false);
//    }
}

int FusionStyle::FusionDarkStyle::styleHint(StyleHint hint, const QStyleOption *option,
                                                  const QWidget *widget,
                                                  QStyleHintReturn *returnData) const
{
    switch (hint)
    {
        case SH_DitherDisabledText:
            return int(false);
        case SH_EtchDisabledText:
            return int(true);
        case SH_GroupBox_TextLabelColor:
            return int(true);
        default:
            return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
}

QPixmap FusionStyle::FusionDarkStyle::generatedIconPixmap(
                          QIcon::Mode iconMode, const QPixmap &pixmap,
                          const QStyleOption *option) const
{
    if (iconMode == QIcon::Disabled)
    {
        QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
        QPainter painter(&img);
        painter.save();
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(img.rect(), QColor(qRgba(160, 160, 160, 0)));
        painter.restore();
        return QPixmap::fromImage(img);
    }

    return QProxyStyle::generatedIconPixmap(iconMode, pixmap, option);
}

void FusionStyle::FusionDarkStyle::drawControl(ControlElement element,
                                                     const QStyleOption *option,
                                                     QPainter *painter,
                                                     const QWidget *widget) const
{
    switch (element)
    {
    case CE_PushButtonLabel:
        {
            QStyleOptionButton myButtonOption;
            const QStyleOptionButton *buttonOption =
                    qstyleoption_cast<const QStyleOptionButton *>(option);
            if (buttonOption) {
                myButtonOption = *buttonOption;
                if (myButtonOption.palette.currentColorGroup()
                        != QPalette::Disabled) {
                    if (myButtonOption.state & (State_Sunken | State_On)) {
                        myButtonOption.palette.setBrush(QPalette::ButtonText,
                                myButtonOption.palette.brightText());
                    }
                }
            }

            QProxyStyle::drawControl(element, &myButtonOption, painter, widget);
        }
        break;
    default:
        QProxyStyle::drawControl(element, option, painter, widget);
    }
}

// Light
void FusionStyle::FusionLightStyle::polish(QWidget *widget)
{
    if (qobject_cast<QPushButton *>(widget)
            || qobject_cast<QComboBox *>(widget))
    {
        widget->setAttribute(Qt::WA_Hover, true);
    }
}

void FusionStyle::FusionLightStyle::unpolish(QWidget *widget)
{
    if (qobject_cast<QPushButton *>(widget)
            || qobject_cast<QComboBox *>(widget))
    {
        widget->setAttribute(Qt::WA_Hover, false);
    }
}

int FusionStyle::FusionLightStyle::styleHint(StyleHint hint, const QStyleOption *option,
                                                  const QWidget *widget,
                                                  QStyleHintReturn *returnData) const
{
    switch (hint)
    {
        case SH_DitherDisabledText:
            return int(false);
        case SH_EtchDisabledText:
            return int(true);
        case SH_GroupBox_TextLabelColor:
            return int(true);
        default:
            return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
}

QPixmap FusionStyle::FusionLightStyle::generatedIconPixmap(
                          QIcon::Mode iconMode, const QPixmap &pixmap,
                          const QStyleOption *option) const
{
    if (iconMode == QIcon::Disabled)
    {
        QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
        QPainter painter(&img);
        painter.save();
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(img.rect(), QColor(qRgba(160, 160, 160, 0)));
        painter.restore();
        return QPixmap::fromImage(img);
    }

    return QProxyStyle::generatedIconPixmap(iconMode, pixmap, option);
}

void FusionStyle::FusionLightStyle::drawControl(ControlElement element,
                                                     const QStyleOption *option,
                                                     QPainter *painter,
                                                     const QWidget *widget) const
{
    switch (element)
    {
    case CE_PushButtonLabel:
        {
            QStyleOptionButton myButtonOption;
            const QStyleOptionButton *buttonOption =
                    qstyleoption_cast<const QStyleOptionButton *>(option);
            if (buttonOption) {
                myButtonOption = *buttonOption;
                if (myButtonOption.palette.currentColorGroup()
                        != QPalette::Disabled) {
                    if (myButtonOption.state & (State_Sunken | State_On)) {
                        myButtonOption.palette.setBrush(QPalette::ButtonText,
                                myButtonOption.palette.brightText());
                    }
                }
            }

            QProxyStyle::drawControl(element, &myButtonOption, painter, widget);
        }
        break;
    default:
        QProxyStyle::drawControl(element, option, painter, widget);
    }
}

QStringList FusionStyle::getStyles()
{
    QStringList styles(QStyleFactory::keys());

    styles.append(QString("Fusion Dark"));
    styles.append(QString("Fusion Light"));
    styles.append(QString("Fusion Zodiac Blue"));

    return styles;
}

void FusionStyle::setstyle(const QString &style, const bool use_standard_palette)
{
    if (use_standard_palette)
        QApplication::setPalette(QApplication::style()->standardPalette());
    else
        QPalette();

    if ("Fusion Dark" == style)
        QApplication::setStyle(new FusionStyle::FusionDarkStyle);
    else if ("Fusion Light" == style)
        QApplication::setStyle(new FusionStyle::FusionLightStyle);
    else if ("Fusion Zodiac Blue" ==  style)
        QApplication::setStyle(new FusionStyle::FusionZodiacBlueStyle);
    else
        QApplication::setStyle(QStyleFactory::create(style));
}

QString FusionStyle::getCurrentStyle()
{
    return QApplication::style()->objectName();
}

