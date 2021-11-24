#pragma once
#include <QProxyStyle>
#include <QPalette>
#include <QStyleFactory>
#include <QApplication>
#include <QtWidgets>
#include <QPainter>


namespace FusionStyle
{
    class FusionDarkStyle : public QProxyStyle
    {
        Q_OBJECT

    public:
        FusionDarkStyle() : QProxyStyle(QStyleFactory::create("Fusion"))
        {  QApplication::style()->setObjectName(QString("Fusion Dark")); }

        void polish(QPalette &palette) override;
        void polish(QWidget *widget) override;
        void unpolish(QWidget *widget) override;

        int styleHint(StyleHint hint, const QStyleOption *option,
                      const QWidget *widget, QStyleHintReturn *returnData) const override;

        QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                    const QStyleOption *option) const override;

        void drawControl(ControlElement control, const QStyleOption *option,
                            QPainter *painter, const QWidget *widget) const override;
    };


    class FusionLightStyle : public QProxyStyle
    {
        Q_OBJECT

    public:
        FusionLightStyle() : QProxyStyle(QStyleFactory::create("Fusion"))
        {  QApplication::style()->setObjectName("Fusion Light"); }

        void polish(QPalette &palette) override;
        void polish(QWidget *widget) override;
        void unpolish(QWidget *widget) override;

        int styleHint(StyleHint hint, const QStyleOption *option,
                      const QWidget *widget, QStyleHintReturn *returnData) const override;

        QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                    const QStyleOption *option) const override;

        void drawControl(ControlElement control, const QStyleOption *option,
                            QPainter *painter, const QWidget *widget) const override;
    };

    class FusionZodiacBlueStyle : public QProxyStyle
    {
        Q_OBJECT

    public:
        FusionZodiacBlueStyle() : QProxyStyle(QStyleFactory::create("Fusion"))
        { QApplication::style()->setObjectName(QString("Fusion Zodiac Blue")); }

        void polish(QPalette &palette) override;
        int styleHint(StyleHint hint, const QStyleOption *option,
                      const QWidget *widget, QStyleHintReturn *returnData) const override;

        QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                    const QStyleOption *option) const override;

        void drawControl(ControlElement control, const QStyleOption *option,
                            QPainter *painter, const QWidget *widget) const override;
    };

    QStringList getStyles();
    void setstyle(const QString& style, const bool use_standard_palette);
    QString getCurrentStyle();
}

