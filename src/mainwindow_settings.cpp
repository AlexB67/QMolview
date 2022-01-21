#include "mainwindow.h"

#ifdef Q_OS_WIN
#include "qcustom/fusionstyles.h"
#endif

#include "appglobals.h"
#include <QPalette>
#include <QStyleFactory>
#include <QSettings>
#include <QFileInfo>
#include <QDir>


void MainWindow::load_settings()
{
    const QString& keyfile = SystemSettings::keyfile;

    if (!QFileInfo::exists(keyfile))
        return;

    QSettings settings(keyfile, QSettings::IniFormat);

#ifdef Q_OS_WIN
    bool desktoptheme_found{false};
    QString str;
#endif

    settings.beginGroup("Appearance");

    if (settings.contains("desktoptheme"))
    {
#ifdef Q_OS_LINUX
        if (settings.contains("standardpalette"))
        {
            if (settings.value("standardpalette").toBool())
                QApplication::setPalette(QApplication::style()->standardPalette());
            else
                QPalette();
        }

        QApplication::setStyle(QStyleFactory::create(settings.value("desktoptheme").toString()));
#endif

#ifdef Q_OS_WIN
        FusionStyle::setstyle(settings.value("desktoptheme").toString(), false);
        desktoptheme_found = true;
        str = settings.value("desktoptheme").toString().toLower();
#endif
    }

    if (settings.contains("iconset"))
    {
        if (settings.value("iconset").toString() == tr("Black"))
            icon_theme_user_changed(true);
        else if (settings.value("iconset").toString() == tr("White"))
            icon_theme_user_changed(false);
        else
            icon_theme_changed();
    }

    if (settings.contains("smallicons"))
       icon_size_changed(settings.value("smallicons").toInt());

    settings.endGroup();

    settings.beginGroup("ViewPort");
#ifdef Q_OS_LINUX
    auto str = settings.value("viewportcolour").toString();

    if (settings.contains("viewportcolour") && SystemSettings::viewport_theme_colours.contains(str))
        view->defaultFrameGraph()->setClearColor(SystemSettings::viewport_theme_colours[str]);

    if (str == tr("Custom") && settings.contains("viewportcustom"))
        view->defaultFrameGraph()->setClearColor(settings.value("viewportcustom").value<QColor>());
#endif

#ifdef Q_OS_WIN
    if (desktoptheme_found)
    {
        if (str == tr("Fusion Light").toLower())
            view->defaultFrameGraph()->setClearColor(QColor(qRgb(191, 222, 233)));
        else if (str == tr("Fusion Dark").toLower())
            view->defaultFrameGraph()->setClearColor(QColor(qRgb(37, 37, 37)));
        else if (str == tr("Fusion Zodiac Blue").toLower())
            view->defaultFrameGraph()->setClearColor(QColor(qRgb(41, 46, 60)));
        else
            view->defaultFrameGraph()->setClearColor(QColor(qRgb(110, 111, 119)));
    }

//    if (settings.contains("viewportcustom"))
//        view->defaultFrameGraph()->setClearColor(settings.value("viewportcustom").value<QColor>());
#endif


    if (settings.contains("viewportintensity"))
    {
        view->set_viewport_Intensity(settings.value("viewportintensity").toInt());
    }

    if (settings.contains("viewportaccel"))
    {
        view->get_cam_controller()->set_camera_mouse_speed(settings.value("viewportaccel").toInt());
    }
    
    settings.endGroup();

    settings.beginGroup("Platform");

    if (settings.contains("menustyle"))
    {
        set_menubar_style(settings.value("menustyle").toInt());

        if (settings.contains("hidemenubar") && settings.value("menustyle").toInt() == 0)
            menuBar()->setHidden(settings.value("hidemenubar").toBool());
    }

    if (settings.contains("nativedialogs"))
        SystemSettings::native_dialogs = settings.value("nativedialogs").toBool();

    settings.endGroup();

}

QIcon MainWindow::set_icon_name(const QString& icon_name)
{
    const bool dark_theme = SystemSettings::theme_is_dark();

    if (dark_theme)
        return QIcon(icon_name + QString(".symbolic.png"));

    return QIcon(icon_name + QString(".symbolic-light.png"));
}

QIcon MainWindow::set_icon_name(const QString& icon_name, const bool dark)
{
    if (dark)
        return QIcon(icon_name + QString(".symbolic.png"));

    return QIcon(icon_name + QString(".symbolic-light.png"));
}

void MainWindow::set_viewport_colour()
{
    // these are defaults, but if the user has changed preferences
    // they will be overridden by qsettings.

#ifdef Q_OS_LINUX
    const QString theme = QApplication::style()->objectName().toLower();

    (SystemSettings::viewport_colour_from_system_theme.contains(theme))
    ? view->defaultFrameGraph()->setClearColor(SystemSettings::viewport_colour_from_system_theme[theme])
    : view->defaultFrameGraph()->setClearColor(SystemSettings::viewport_colour_from_system_theme["default"]);
#endif

#ifdef Q_OS_WIN
     FusionStyle::setstyle("Fusion Zodiac Blue", false);
     view->defaultFrameGraph()->setClearColor(QColor(qRgb(41, 46, 60)));
     icon_theme_changed(); // defaults
#endif

}




