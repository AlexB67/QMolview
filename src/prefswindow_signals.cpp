#include "prefswindow.h"

#ifdef Q_OS_WIN
#include "fusionstyles.h"
#endif

#include <QPalette>
#include <QStyleFactory>
#include <QtWidgets/QApplication>


void PrefsWindow::changeTheme()
{

#ifdef Q_OS_LINUX
    QApplication::setStyle(QStyleFactory::create(themeselect->currentText()));

    if (standard_palette->isChecked())
        QApplication::setPalette(QApplication::style()->standardPalette());
    else
        QPalette();
#endif

#ifdef Q_OS_WIN
    FusionStyle::setstyle(themeselect->currentText(),false);
#endif

#ifdef Q_OS_LINUX
    if (themeselect->currentText() == tr("adwaita"))
        viewportcolour->setCurrentText(tr("Adwaita"));
    else if (themeselect->currentText() == tr("adwaita-dark"))
        viewportcolour->setCurrentText(tr("Adwaita-dark"));
    else if (themeselect->currentText() == tr("breeze"))
        viewportcolour->setCurrentText(tr("Breeze"));
    else if (themeselect->currentText() == tr("layan-solid") || (themeselect->currentText() == tr("layan")))
        viewportcolour->setCurrentText(tr("Layan"));
    else if (themeselect->currentText() == tr("kvantum") || themeselect->currentText() == tr("kvantum-dark"))
        viewportcolour->setCurrentText(tr("Default"));
    else if (themeselect->currentText() == tr("fusion"))
        viewportcolour->setCurrentText(tr("Fusion"));
#endif

#ifdef Q_OS_WIN
    if (themeselect->currentText().toLower() == tr("Fusion Light").toLower())
        Q_EMIT viewPortColour_changed(QColor(qRgb(191, 222, 233)));
    else if (themeselect->currentText().toLower() == tr("Fusion Dark").toLower())
        Q_EMIT viewPortColour_changed(QColor(qRgb(37, 37, 37)));
    else if (themeselect->currentText().toLower() == tr("Fusion Zodiac Blue").toLower())
        Q_EMIT viewPortColour_changed(QColor(qRgb(41, 46, 60)));
    else
        Q_EMIT viewPortColour_changed(QColor(qRgb(110, 111, 119)));
#endif

    Q_EMIT themePrefs_changed();
}

void PrefsWindow::changeMenu()
{
    Q_EMIT hideMenuBar_changed(hide_menu);
}

void PrefsWindow::changeIcons()
{
    if (iconselect->currentText() == tr("Auto"))
        Q_EMIT iconColour_changed(true, false);
    else if (iconselect->currentText() == tr("Black"))
        Q_EMIT iconColour_changed(false, false);
    else
        Q_EMIT iconColour_changed(false, true);
}

void PrefsWindow::changeViewPort()
{

#ifdef Q_OS_LINUX
    if (viewportcolour->currentText() == tr("Black"))
        Q_EMIT viewPortColour_changed(QColor(qRgb(0, 0, 0)));
    else if (viewportcolour->currentText() == tr("Breeze"))
        Q_EMIT viewPortColour_changed(QColor(qRgb(35, 38, 41)));
    else if (viewportcolour->currentText() == tr("Adwaita-dark"))
         Q_EMIT viewPortColour_changed(QColor(qRgb(37, 37, 37)));
    else if (viewportcolour->currentText() == tr("Adwaita"))
         Q_EMIT viewPortColour_changed(QColor(qRgb(110, 111, 119)));
    else if (viewportcolour->currentText() == tr("Layan"))
         Q_EMIT viewPortColour_changed(QColor(qRgb(68, 69, 75)));
    else if (viewportcolour->currentText() == tr("Fusion") && standard_palette->isChecked())
         Q_EMIT viewPortColour_changed(QColor(qRgb(68, 69, 75)));
    else
         Q_EMIT viewPortColour_changed(QColor(QRgb(0x4d4d4f)));
#endif

}
