#include "appglobals.h"
#include "prefswindow.h"

#ifdef Q_OS_WIN
#include "qcustom/fusionstyles.h"
#endif

#include <QPalette>
#include <QStyleFactory>
#include <QtWidgets/QApplication>
#include <QColorDialog>
#include <QFontDialog>

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
    if (SystemSettings::theme_to_view_colour.contains(themeselect->currentText()))
        viewportcolour->setCurrentText(SystemSettings::theme_to_view_colour[themeselect->currentText()]);
    else
        viewportcolour->setCurrentText(tr("Default"));
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
    Q_EMIT chart_Theme_Changed(chart_theme->currentIndex());
}

void PrefsWindow::changeMenuBarVisible()
{
    Q_EMIT hideMenuBar_changed(hide_menu);
}

void PrefsWindow::changeMenuStyle()
{
    hide_menu->blockSignals(true);

    if (menu_style->currentIndex() == 0)
    {
        hide_menu->setEnabled(true);
        hide_menu_label->setEnabled(true);
        hide_menu->setChecked(false);
    }
    else
    {
        hide_menu->setEnabled(false);
        hide_menu_label->setEnabled(false);
        hide_menu->setChecked(true);
    }

    hide_menu->blockSignals(false);

    Q_EMIT MenuBarStyle_changed(menu_style->currentIndex());
}

void PrefsWindow::changeDialogStyle()
{
    Q_EMIT dialogStyle_changed(dialog_style->isChecked());
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

void PrefsWindow::changeIconSize()
{
    Q_EMIT iconSize_Changed(icon_size->currentIndex());
}

void PrefsWindow::changeViewPort()
{

#ifdef Q_OS_LINUX
    if (viewportcolour->currentText() != tr("Custom"))
    {
        if (SystemSettings::viewport_theme_colours.contains(viewportcolour->currentText()))
            Q_EMIT viewPortColour_changed(SystemSettings::viewport_theme_colours[viewportcolour->currentText()]);
        else // should not really be needed, but anyway, a fallback
            Q_EMIT viewPortColour_changed(QColor(QRgb(0x4d4d4f)));
    }
#endif
}

void PrefsWindow::changeViewPortIntensity()
{
    intensity_value->setText(QString::number(intensity->value()) + QString("%"));
    Q_EMIT viewport_Intensity_changed(intensity->value());
}

void PrefsWindow::changeMouseSpeed()
{
    Q_EMIT mouse_speed_changed(mouse_speed->value());
}

void PrefsWindow::set_spectrum_axes_title_font()
{
    QFontDialog::FontDialogOptions options;

    if (!SystemSettings::native_dialogs)
        options = QFlag(QFontDialog::DontUseNativeDialog);

    QFontDialog *dialog = new QFontDialog(this);

    bool ok;
    QFont font = dialog->getFont(&ok, QFont("Nimbus [Roman]", 14), this, tr("Select an axis title font"), options);

    if (ok)
    {
        spectrum_axes_title_font->setText(font.family() + QString("  ")
                                          + QString::number(font.pointSize()));
        Q_EMIT spectrum_axis_title_font_Changed(font);
    }

    delete dialog;
}

void PrefsWindow::set_spectrum_axes_label_font()
{
    QFontDialog::FontDialogOptions options;

    if (!SystemSettings::native_dialogs)
        options = QFlag(QFontDialog::DontUseNativeDialog);

    QFontDialog *dialog = new QFontDialog(this);

    bool ok;
    QFont font = dialog->getFont(&ok, QFont("Nimbus [Roman]", 11), this, tr("Select an axis label font"), options);

    if (ok)
    {
        spectrum_axes_label_font->setText(font.family() + QString("  ")
                                          + QString::number(font.pointSize()));
        Q_EMIT spectrum_axis_label_font_Changed(font);
    }

    delete dialog;
}

void PrefsWindow::set_spectrum_line_colour()
{
    QColorDialog::ColorDialogOptions options;

    if (!SystemSettings::native_dialogs)
        options = QFlag(QColorDialog::DontUseNativeDialog);

    QColorDialog *dialog = new QColorDialog(this);
    dialog->setModal(true);
    dialog->setCustomColor(0, QColor(153, 210, 255));
    dialog->setCustomColor(1, QColor(10, 10, 10));

    const QColor colour = dialog->getColor(spectrum_line_colour->Colour(), this, "Select Color", options);

    if(colour.isValid() && !auto_line_colour->isChecked())
    {
        spectrum_line_colour->setColour(colour);
        Q_EMIT spectrum_line_Colour_Changed(spectrum_line_colour->Colour());
    }

    delete dialog;
}

void PrefsWindow::set_spectrum_auto_line_colour()
{
    spectrum_line_colour->setEnabled(!auto_line_colour->isChecked());
    Q_EMIT spectrum_auto_line_Colour_Changed(auto_line_colour->isChecked());

    if (spectrum_line_colour->Colour().isValid())
        Q_EMIT spectrum_line_Colour_Changed(spectrum_line_colour->Colour());
}

void PrefsWindow::set_spectrum_title_viisible()
{
    Q_EMIT spectrum_titleVisible_Changed(spectrum_title->isChecked());
}

void PrefsWindow::set_chart_theme()
{
    spectrum_line_colour->setEnabled(false);
    Q_EMIT chart_Theme_Changed(chart_theme->currentIndex());
    auto_line_colour->setChecked(true);
}

void PrefsWindow::set_viewport_colour_custom()
{
    QColorDialog::ColorDialogOptions options;

    if (!SystemSettings::native_dialogs)
        options = QFlag(QColorDialog::DontUseNativeDialog);

    QColorDialog *dialog = new QColorDialog(this);
    dialog->setModal(true);
    dialog->setCustomColor(0, QRgb(0x4d4d4f));

    const QColor colour = dialog->getColor(viewportcolour_custom->Colour(), this, "Select Color", options);

    if(colour.isValid())
    {
        Q_EMIT viewPortColour_changed(colour);
        viewportcolour_custom->setColour(colour);
        viewportcolour->setCurrentIndex(7);
    }

    delete dialog;
}

void PrefsWindow::set_connectivity_index()
{
    Q_EMIT connectivity_Index_Changed(connectivity_frame_num->currentIndex() == 1);
}

void PrefsWindow::set_sort_axes()
{
    Q_EMIT sort_Axes_Changed(sort_axes->isChecked());
}

void PrefsWindow::set_align_axes()
{
    Q_EMIT align_Axes_Changed(align_axes->isChecked());
}

