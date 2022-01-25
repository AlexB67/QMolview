#include "appglobals.h"
#include "prefswindow.h"
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QSettings>

#include "appglobals.h"
#ifdef Q_OS_WIN
#include "qcustom/fusionstyles.h"
#else
#include <QStyleFactory>
#include <QApplication>
#endif

void PrefsWindow::save_settings()
{
    const QString& keyfile = SystemSettings::keyfile;

    QSettings settings(keyfile, QSettings::IniFormat);

    QString group("Appearance");
    settings.beginGroup(group);
    
    settings.setValue("desktoptheme", themeselect->currentText());
    settings.setValue("standardpalette", standard_palette->isChecked());
    settings.setValue("iconset", iconselect->currentText());
    settings.setValue("smallicons", icon_size->currentIndex());
    
    settings.endGroup();

    QString group2("ViewPort");
    settings.beginGroup(group2);

#ifdef Q_OS_LINUX
    settings.setValue("viewportcolour", viewportcolour->currentText());
#endif
    settings.setValue("viewportcustom", viewportcolour_custom->Colour());
    settings.setValue("viewportintensity", intensity->value());
    settings.setValue("viewportaccel", mouse_speed->value());
    settings.endGroup();

// Removed spectrum settings. Now done by spectrum window

    QString group4("Platform");
    settings.beginGroup(group4);
    settings.setValue("menustyle", menu_style->currentIndex());
    settings.setValue("hidemenubar", hide_menu->isChecked());
    settings.setValue("nativedialogs", dialog_style->isChecked());
    settings.endGroup();
}

void PrefsWindow::load_settings()
{
    const QString& keyfile = SystemSettings::keyfile;

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

    QString group("Appearance");
    settings.beginGroup(group);

    if (settings.contains("desktoptheme"))
        themeselect->setCurrentText(settings.value("desktoptheme").toString());

    if (settings.contains("standardpalette"))
            standard_palette->setChecked(settings.value("standardpalette").toBool());

    if (settings.contains("iconset"))
        iconselect->setCurrentText(settings.value("iconset").toString());

    if (settings.contains("smallicons"))
        icon_size->setCurrentIndex(settings.value("smallicons").toInt());

    settings.endGroup();

    QString group2("ViewPort");
    settings.beginGroup(group2);

#ifdef Q_OS_LINUX
    if (settings.contains("viewportcolour"))
        viewportcolour->setCurrentText(settings.value("viewportcolour").toString());
#endif

    if (settings.contains("viewportcustom"))
        viewportcolour_custom->setColour(settings.value("viewportcustom").value<QColor>());

    if (settings.contains("viewportintensity"))
        intensity->setValue(settings.value("viewportintensity").toInt());

    if (settings.contains("viewportaccel"))
        mouse_speed->setValue(settings.value("viewportaccel").toInt());

    settings.endGroup();

    QString group3("Spectrum");
    settings.beginGroup(group3);

    if (settings.contains("charttheme"))
        chart_theme->setCurrentIndex(settings.value("charttheme").toInt());

    if (settings.contains("autocolour"))
        auto_line_colour->setChecked(settings.value("autocolour").toBool());

    if (settings.contains("linecolour"))
        spectrum_line_colour->setColour(settings.value("linecolour").value<QColor>());

    if (settings.contains("axistitlefont"))
        spectrum_axes_title_font->setFont(settings.value("axistitlefont").value<QFont>());

    if (settings.contains("axislabelfont"))
        spectrum_axes_label_font->setFont(settings.value("axislabelfont").value<QFont>());

    if (settings.contains("showtitle"))
        spectrum_title->setChecked(settings.value("showtitle").toBool());

    settings.endGroup();


    QString group4("Platform");
    settings.beginGroup(group4);

    if (settings.contains("menustyle"))
        menu_style->setCurrentIndex(settings.value("menustyle").toInt());

    if (settings.contains("hidemenubar") && menu_style->currentIndex() == 0)
        hide_menu->setChecked(settings.value("hidemenubar").toBool());
    else
        hide_menu->setChecked(true);

    if (settings.contains("nativedialogs"))
        dialog_style->setChecked(settings.value("nativedialogs").toBool());

    settings.endGroup();
}

void PrefsWindow::removeAppearanceSettings()
{
    const QString& keyfile = SystemSettings::keyfile;

    if (!QFileInfo::exists(keyfile)) return;

    QSettings settings(keyfile, QSettings::IniFormat);

    QString group("Appearance");
    settings.beginGroup(group);
    settings.remove("");
    settings.endGroup();

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
    icon_size->setCurrentIndex(1); // medium
}

void PrefsWindow::removeViewPortSettings()
{
    const QString& keyfile = SystemSettings::keyfile;

    if (!QFileInfo::exists(keyfile)) return;

    QSettings settings(keyfile, QSettings::IniFormat);

    QString group("ViewPort");
    settings.beginGroup(group);
    settings.remove("");
    settings.endGroup();

    intensity->setValue(100);
#ifdef Q_OS_LINUX
    viewportcolour->setCurrentText("Default");
#endif

    mouse_speed->setValue(100);
}

void PrefsWindow::removeSpectrumSettings()
{
    const QString& keyfile = SystemSettings::keyfile;

    if (!QFileInfo::exists(keyfile)) return;

    QSettings settings(keyfile, QSettings::IniFormat);

    QString group("Spectrum");
    settings.beginGroup(group);
    settings.remove("");
    settings.endGroup();

    chart_theme->setCurrentIndex(8);
    spectrum_title->setChecked(true);
    auto_line_colour->setChecked(true);


#ifdef Q_OS_LINUX
    spectrum_axes_title_font->setFont(QFont(tr("Nimbus Roman"), 14));
#else
    spectrum_axes_title_font->setFont(QFont(tr("Times New Roman"), 14));
#endif

#ifdef Q_OS_LINUX
    spectrum_axes_label_font->setFont(QFont(tr("Nimbus Roman"), 11));
#else
    spectrum_axes_label_font->setFont(QFont(tr("Times New Roman"), 11));
#endif

    Q_EMIT spectrum_axis_title_font_Changed(spectrum_axes_title_font->font());
    Q_EMIT spectrum_axis_label_font_Changed(spectrum_axes_label_font->font());
}

void PrefsWindow::removeSystemSettings()
{
    const QString& keyfile = SystemSettings::keyfile;

    if (!QFileInfo::exists(keyfile)) return;

    QSettings settings(keyfile, QSettings::IniFormat);

    QString group("Platform");
    settings.beginGroup(group);
    settings.remove("");
    settings.endGroup();

    const QString desktop = SystemSettings::desktop_environment();

    if (desktop.contains("gnome", Qt::CaseInsensitive))
    {
        menu_style->setCurrentIndex(1);
        hide_menu->setEnabled(false);
        hide_menu_label->setEnabled(false);
        hide_menu->setChecked(true);
    }
    else
    {
        menu_style->setCurrentIndex(0);
        hide_menu->setEnabled(true);
        hide_menu_label->setEnabled(true);
        hide_menu->setChecked(false);
    }

     dialog_style->setChecked(false);
}

