#ifndef PREFSWINDOW_H
#define PREFSWINDOW_H

#include "qcustom/colour_button.h"
#include <QObject>
#include <QWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QEvent>

using QCustom::ColourButton;

class PrefsWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PrefsWindow(QWidget *parent = nullptr);

    void save_settings();
    void load_settings();

Q_SIGNALS:
    void themePrefs_changed();
    void hideMenuBar_changed(QCheckBox *);
    void MenuBarStyle_changed(int);
    void dialogStyle_changed(bool);
    void viewPortColour_changed(QColor viewport_colour);
    void mouse_speed_changed(int);
    void iconColour_changed(bool, bool);
    void viewport_Intensity_changed(int);
    void iconSize_Changed(int);
    void chart_Theme_Changed(int);
    void spectrum_line_Colour_Changed(const QColor&);
    void spectrum_auto_line_Colour_Changed(bool);
    void spectrum_titleVisible_Changed(bool);
    void spectrum_axis_title_font_Changed(const QFont&);
    void spectrum_axis_label_font_Changed(const QFont&);
    // file import export
    void connectivity_Index_Changed(bool);
    void sort_Axes_Changed(bool);
    void align_Axes_Changed(bool);

private:
    QGroupBox   *appearanceBox;
    QGroupBox   *viewPortBox;
    QGroupBox   *spectrumBox;
    QGroupBox   *systemBox;
    QGroupBox   *fileBox;
    QVBoxLayout *vboxLayout;
    QGridLayout *appearancegrid;
    QGridLayout *viewPortgrid;
    QGridLayout *spectrumgrid;
    QGridLayout *systemGrid;
    QGridLayout *fileGrid;
    QTabWidget  *tabwidget;
    QComboBox   *themeselect;
    QComboBox   *viewportcolour;
    QComboBox   *iconselect;
    QComboBox   *icon_size;
    QComboBox   *chart_theme;
    QComboBox   *menu_style;
    QComboBox   *connectivity_frame_num;
    QCheckBox   *standard_palette;
    QCheckBox   *hide_menu;
    QCheckBox   *dialog_style;
    QCheckBox   *auto_line_colour;
    QCheckBox   *spectrum_title;
    QCheckBox   *sort_axes;
    QCheckBox   *align_axes;
    QSlider     *intensity;
    QSlider     *mouse_speed;
    ColourButton  *spectrum_line_colour;
    ColourButton  *viewportcolour_custom;
    QPushButton  *spectrum_axes_title_font;
    QPushButton  *spectrum_axes_label_font;
    QPushButton *restore_appearance_defaults;
    QPushButton *restore_viewport_defaults;
    QPushButton *restore_spectrum_defaults;
    QPushButton *restore_system_defaults;
    QLabel      *theme_label;
    QLabel      *chart_theme_label;
    QLabel      *icon_label;
    QLabel      *viewport_label;
    QLabel      *intensity_label;
    QLabel      *mouse_speed_label;
    QLabel      *intensity_value;
    QLabel      *icon_size_label;
    QLabel      *spectrum_line_colour_label;
    QLabel      *spectrum_axes_title_font_label;
    QLabel      *spectrum_axes_label_font_label;
    QLabel      *spectrum_title_label;
    QLabel      *menu_style_label;
    QLabel      *hide_menu_label;
    QLabel      *current_desktop;
    QLabel      *connectivity_frame_num_label;

    void create_appearance_tab();
    void create_viewport_tab();
    void create_spectrum_tab();
    void create_system_tab();
    void create_files_tab();

    void changeEvent(QEvent *) override;
    void closeEvent(QCloseEvent *) override;
    void keyPressEvent(QKeyEvent *) override;

private Q_SLOTS:
    // Main window, dialogs
    void changeTheme();
    void changeMenuBarVisible();
    void changeMenuStyle();
    void changeDialogStyle();
    void changeViewPort();
    void changeViewPortIntensity();
    void changeIcons();
    void changeIconSize();
    void changeMouseSpeed();
    void removeAppearanceSettings();
    void removeViewPortSettings();
    void removeSpectrumSettings();
    void removeSystemSettings();

    // Spectrum Window
    void set_spectrum_axes_title_font();
    void set_spectrum_axes_label_font();
    void set_spectrum_line_colour();
    void set_spectrum_auto_line_colour();
    void set_chart_theme();
    void set_spectrum_title_viisible();
    void set_viewport_colour_custom();

    // file import/export
    void set_connectivity_index();
    void set_align_axes();
    void set_sort_axes();
};

#endif // PREFSWINDOW_H
