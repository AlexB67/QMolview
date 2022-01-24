#ifndef APPGLOBALS_H
#define APPGLOBALS_H
#include <QString>
#include <QGuiApplication>
#include <QPalette>
#include <QColor>
#include <QDir>
#include <QFont>
#include <QtMath>
#include <QMap>

namespace SystemSettings
{
    inline QString systemstyle;
    inline bool native_dialogs{false};
    inline QFont systemFont;
    inline QString install_preFix;

// need a better way for flatpak paths, will have to do for now & testing

#ifdef FLATPAK
    inline QString keyfile = QDir::homePath() + QString("/.config/qmolview/.qmolview_flatpak_qt"
                           + QString::number(QT_VERSION_MAJOR) + ".conf");
#else
    inline QString keyfile = QDir::homePath() + QString("/.config/qmolview/.qmolview_qt"
                           + QString::number(QT_VERSION_MAJOR) + ".conf");
#endif

    inline static const QMap<QString, QColor> viewport_theme_colours
    {
        {QString("Adwaita-dark"), QColor(qRgb(37, 37, 37))},
        {QString("Adwaita"), QColor(qRgb(110, 111, 119))},
        {QString("Black"), QColor(qRgb(0, 0, 0))},
        {QString("Breeze"), QColor(qRgb(27, 30, 32))},
        {QString("Default"), QColor(QRgb(0x4d4d4f))},
        {QString("Layan"), QColor(qRgb(60, 60, 69))},
        {QString("Layan-solid"), QColor(qRgb(60, 60, 69))},
        {QString("Fusion"), QColor(qRgb(68, 69, 75))},
        {QString("Custom"), QColor(qRgb(0, 0, 0))}
    };
    // TODO: We could add a few more
    inline static const QMap<QString, QColor> viewport_colour_from_system_theme
    {
        {QString("adwaita-dark"), QColor(qRgb(37, 37, 37))},
        {QString("adwaita"), QColor(qRgb(110, 111, 119))},
        {QString("breeze"), QColor(qRgb(27, 30, 32))},
        {QString("layan"), QColor(qRgb(60, 60, 69))},
        {QString("layan-solid"), QColor(qRgb(60, 60, 69))},
        {QString("fusion"), QColor(qRgb(68, 69, 75))},
        {QString("default"), QColor(QRgb(0x4d4d4f))},
    };

    // The decision was made to treat system themes lower case (LINUX only)
    // objectName is not always conssient with case convention
    // view port colours were left case sensitive
    // WE set a viewport colour mapping derived from the system theme
    inline static const QMap<QString, QString> theme_to_view_colour
    {
        {QString("adwaita-dark"), QString("Adwaita-dark")},
        {QString("adwaita"), QString("Adwaita")},
        {QString("breeze"), QString("Breeze")},
        {QString("layan"), QString("Layan")},
        {QString("layan-solid"), QString("Layan-solid")},
        {QString("fusion"), QString("Fusion")},
        {QString("default"), QString("Default")},
    };

    inline bool theme_is_dark()
    {
        const QColor textColor = QGuiApplication::palette().color(QPalette::Text);

        if (qSqrt(((textColor.red() * textColor.red()) * 0.299) +
                  ((textColor.green() * textColor.green()) * 0.587) +
                  ((textColor.blue() * textColor.blue()) * 0.114)) > 128)
            return true;
        else
            return false;
    }

    inline QString desktop_environment()
    {
        QString desktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
        if (desktop.isEmpty()) desktop = qEnvironmentVariable("XDG_SESSION_DESKTOP");
        if (desktop.isEmpty()) desktop = qEnvironmentVariable("DESKTOP_SESSION");
        if (desktop.isEmpty()) desktop = "Unknown";

        return desktop;
    }
}

#endif // APPGLOBALS_H

//        if (str == tr("Black"))
//            view->defaultFrameGraph()->setClearColor(QColor(qRgb(0, 0, 0)));
//        else if (str == tr("Breeze"))
//            view->defaultFrameGraph()->setClearColor(QColor(qRgb(27, 30, 32)));
//        else if (str == tr("Adwaita-dark"))
//            view->defaultFrameGraph()->setClearColor(QColor(qRgb(37, 37, 37)));
//        else if (str == tr("Adwaita"))
//            view->defaultFrameGraph()->setClearColor(QColor(qRgb(110, 111, 119)));
//        else if (str == tr("Layan"))
//            view->defaultFrameGraph()->setClearColor(QColor(qRgb(60, 60, 69)));
//        else if (str == tr("Fusion"))
//            view->defaultFrameGraph()->setClearColor(QColor(qRgb(68, 69, 75)));
//        else if (str != tr("Default"))
//            view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
