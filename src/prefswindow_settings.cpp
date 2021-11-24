#include "appglobals.h"
#include "prefswindow.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QSettings>

#ifdef Q_OS_WIN
#include "fusionstyles.h"
#else
#include <QStyleFactory>
#include <QApplication>
#endif

void PrefsWindow::save_settings()
{
    QString path = QDir::homePath();
    QString keyfile = path + QString("/.qmolview_qt5.conf");

    QSettings settings(keyfile, QSettings::IniFormat);

    QString group("General");
    settings.beginGroup(group);
    settings.setValue("desktoptheme", themeselect->currentText());
    settings.setValue("hidemenu", hide_menu->isChecked());

#ifdef Q_OS_LINUX
    settings.setValue("viewportcolour", viewportcolour->currentText());
#endif

    settings.setValue("iconset", iconselect->currentText());
    settings.endGroup();

}

void PrefsWindow::load_settings()
{
    QString path = QDir::homePath();
    QString keyfile = path + QString("/.qmolview_qt5.conf");

    if (!QFileInfo::exists(keyfile))
    {
#ifdef Q_OS_WIN
        themeselect->setCurrentText(tr("Fusion Zodiac Blue"));
#else
        themeselect->setCurrentText(QApplication::style()->objectName());
#endif

        return;
    }

    QSettings settings(keyfile, QSettings::IniFormat);

    QString group("General");
    settings.beginGroup(group);

    if (settings.contains("desktoptheme"))
        themeselect->setCurrentText(settings.value("desktoptheme").toString());

    if (settings.contains("hidemenu"))
        hide_menu->setChecked(settings.value("hidemenu").toBool());

#ifdef Q_OS_LINUX
    if (settings.contains("viewportcolour"))
        viewportcolour->setCurrentText(settings.value("viewportcolour").toString());
#endif
    if (settings.contains("iconset"))
        iconselect->setCurrentText(settings.value("iconset").toString());

    settings.endGroup();
}

void PrefsWindow::removeSettings()
{
    QString path = QDir::homePath();
    QString keyfile = path + QString("/.qmolview_qt5.conf");

    if (!QFileInfo::exists(keyfile)) return;

    QSettings settings(keyfile, QSettings::IniFormat);

    QString group("General");
    settings.beginGroup(group);
    settings.remove("");
    settings.endGroup();

    iconselect->setCurrentText(tr("Auto"));

#ifdef Q_OS_WIN
    themeselect->setCurrentText("Fusion Zodiac Blue");
    standard_palette->setChecked(false);
#else
    QApplication::setStyle(QStyleFactory::create(SystemSettings::systemstyle));
    themeselect->setCurrentText(QApplication::style()->objectName().toLower());
    standard_palette->setChecked(true);
#endif

    hide_menu->setChecked(false);
    iconselect->setCurrentText(tr("Auto"));
}
