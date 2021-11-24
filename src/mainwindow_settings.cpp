#include "mainwindow.h"

#ifdef Q_OS_WIN
#include "fusionstyles.h"
#endif

#include "appglobals.h"
#include <QPalette>
#include <QStyleFactory>
#include <QSettings>
#include <QFileInfo>
#include <QDir>


void MainWindow::load_settings()
{
    QString path = QDir::homePath();
    QString keyfile = path + QString("/.qmolview_qt5.conf");

    if (!QFileInfo::exists(keyfile)) return;

    QSettings settings(keyfile, QSettings::IniFormat);

    settings.beginGroup("General");

    if (settings.contains("desktoptheme"))
    {
        QApplication::setStyle(QStyleFactory::create(settings.value("desktoptheme").toString()));

#ifdef Q_OS_LINUX
        QApplication::setPalette(QApplication::style()->standardPalette());
#endif
    }

    if (settings.contains("hidemenu"))
        menuBar()->setVisible(!settings.value("hidemenu").toBool());

#ifdef Q_OS_LINUX
    auto str = settings.value("viewportcolour").toString();

    if (settings.contains("viewportcolour"))
    {
        if (str == tr("Black"))
            view->defaultFrameGraph()->setClearColor(QColor(qRgb(0, 0, 0)));
        else if (str == tr("Breeze"))
            view->defaultFrameGraph()->setClearColor(QColor(qRgb(35, 38, 41)));
        else if (str == tr("Adwaita-dark"))
            view->defaultFrameGraph()->setClearColor(QColor(qRgb(37, 37, 37)));
        else if (str == tr("Adwaita"))
            view->defaultFrameGraph()->setClearColor(QColor(qRgb(110, 111, 119)));
        else if (str == tr("Layan"))
            view->defaultFrameGraph()->setClearColor(QColor(qRgb(68, 69, 75)));
        else if (str == tr("Fusion"))
            view->defaultFrameGraph()->setClearColor(QColor(qRgb(68, 69, 75)));
        else
            view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
    }
#endif

#ifdef Q_OS_WIN
    auto str = settings.value("desktoptheme").toString().toLower();

    if (str == tr("Fusion Light").toLower())
        view->defaultFrameGraph()->setClearColor(QColor(qRgb(191, 222, 233)));
    else if (str == tr("Fusion Dark").toLower())
        view->defaultFrameGraph()->setClearColor(QColor(qRgb(37, 37, 37)));
    else if (str == tr("Fusion Zodiac Blue").toLower())
        view->defaultFrameGraph()->setClearColor(QColor(qRgb(41, 46, 60)));
    else
        view->defaultFrameGraph()->setClearColor(QColor(qRgb(110, 111, 119)));
#endif

    if (settings.contains("iconset"))
    {
        if (settings.value("iconset").toString() == tr("Black"))
            icon_theme_user_changed(true);
        else if (settings.value("iconset").toString() == tr("White"))
            icon_theme_user_changed(false);
        else
            icon_theme_changed();
    }


    settings.endGroup();
}

bool MainWindow::is_dark_theme()
{
    bool dark_theme = false;

    const auto bg_colour = QWidget::palette().color(QWidget::backgroundRole());

    if (bg_colour.red() < 125 || bg_colour.green() < 125 || bg_colour.blue() < 125)
        dark_theme = true;

    return dark_theme;
}

QIcon MainWindow::set_icon_name(const QString& icon_name)
{
    const bool dark_theme = is_dark_theme();

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

void  MainWindow::set_viewport_colour()
{
    // these are defaults, but if the user has changed preferences
    // they will be overridden by qsettings.

#ifdef Q_OS_LINUX
      const QString theme = QApplication::style()->objectName().toLower();
      SystemSettings::systemstyle = theme;

     if (theme == tr("breeze"))
         view->defaultFrameGraph()->setClearColor(QColor(qRgb(35, 38, 41)));
     else if (theme == tr("adwaita-dark"))
          view->defaultFrameGraph()->setClearColor(QColor(qRgb(37, 37, 37)));
     else if (theme == tr("adwaita"))
         view->defaultFrameGraph()->setClearColor(QColor(qRgb(110, 111, 119)));
     else if (theme == tr("layan")  || theme == tr("layan-solid"))
         view->defaultFrameGraph()->setClearColor(QColor(qRgb(68, 69, 75)));
     else
        view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
#endif

#ifdef Q_OS_WIN
     QApplication::setStyle(new FusionStyle::FusionZodiacBlueStyle);
     view->defaultFrameGraph()->setClearColor(QColor(qRgb(41, 46, 60)));
#endif

}




