#include "appglobals.h"
#include "prefswindow.h"

#ifdef Q_OS_WIN
#include "qcustom/fusionstyles.h"
#endif

#include <QtWidgets/QApplication>
#include <QPalette>
#include <QStyleFactory>
#include <QKeyEvent>
#include <QtCharts>
#include <QtWidgets/QColorDialog>
#include <QSysInfo>

PrefsWindow::PrefsWindow(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::Window);

    setWindowTitle(tr("Preferences"));

    vboxLayout = new QVBoxLayout(this);
    vboxLayout->setContentsMargins(5, 10, 5, 5);

    create_appearance_tab();
    create_viewport_tab();
    create_spectrum_tab();
    create_system_tab();
    create_files_tab();

    tabwidget = new QTabWidget(this);
#ifdef Q_OS_LINUX
    tabwidget->setMinimumWidth(515);
#endif
    tabwidget->addTab(appearanceBox, tr("Appearance"));
    tabwidget->addTab(viewPortBox, tr("3D Viewport"));
    tabwidget->addTab(spectrumBox, tr("IR Spectrum"));
    tabwidget->addTab(systemBox, tr("Platform"));
    tabwidget->addTab(fileBox, tr("Files"));
    vboxLayout->addWidget(tabwidget);

    setLayout(vboxLayout);
    vboxLayout->setSizeConstraint(QLayout::SetFixedSize);

    load_settings(); // before we make any connections

    QObject::connect(themeselect, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &PrefsWindow::changeTheme);

#ifdef Q_OS_LINUX
    QObject::connect(viewportcolour, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &PrefsWindow::changeViewPort);
#endif

    QObject::connect(hide_menu, &QCheckBox::clicked,
                     this, &PrefsWindow::changeMenuBarVisible);

    QObject::connect(dialog_style, &QCheckBox::clicked,
                     this, &PrefsWindow::changeDialogStyle);

    QObject::connect(menu_style, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &PrefsWindow::changeMenuStyle);

    QObject::connect(iconselect, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &PrefsWindow::changeIcons);

#ifdef Q_OS_WIN
    viewport_label->setEnabled(false);
    viewportcolour->setEnabled(false);
    standard_palette->setChecked(false);
    standard_palette->setEnabled(false);
    viewport_label->setToolTip(tr("Linux only."));
    viewportcolour->setToolTip(tr("Linux only."));
    standard_palette->setToolTip(tr("Linux only."));
#endif

    QObject::connect(restore_appearance_defaults, &QPushButton::clicked,
                     this, &PrefsWindow::removeAppearanceSettings);

    QObject::connect(restore_viewport_defaults, &QPushButton::clicked,
                     this, &PrefsWindow::removeViewPortSettings);

    QObject::connect(restore_spectrum_defaults, &QPushButton::clicked,
                     this, &PrefsWindow::removeSpectrumSettings);

    QObject::connect(restore_system_defaults, &QPushButton::clicked,
                     this, &PrefsWindow::removeSystemSettings);

    QObject::connect(intensity, &QSlider::valueChanged,
                     this, &PrefsWindow::changeViewPortIntensity);

    QObject::connect(mouse_speed, &QSlider::valueChanged,
                     this, &PrefsWindow::changeMouseSpeed);

    QObject::connect(icon_size, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &PrefsWindow::changeIconSize);

    QObject::connect(chart_theme, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &PrefsWindow::set_chart_theme);

    QObject::connect(viewportcolour_custom, &QPushButton::clicked,
                     this, &PrefsWindow::set_viewport_colour_custom);

    QObject::connect(spectrum_line_colour, &QPushButton::clicked,
                     this, &PrefsWindow::set_spectrum_line_colour);

    QObject::connect(auto_line_colour, &QCheckBox::clicked,
                     this, &PrefsWindow::set_spectrum_auto_line_colour);

    QObject::connect(spectrum_title, &QCheckBox::clicked,
                     this, &PrefsWindow::set_spectrum_title_viisible);

    QObject::connect(spectrum_axes_title_font, &QPushButton::clicked,
                     this, &PrefsWindow::set_spectrum_axes_title_font);

    QObject::connect(spectrum_axes_label_font, &QPushButton::clicked,
                     this, &PrefsWindow::set_spectrum_axes_label_font);

    QObject::connect(connectivity_frame_num, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &PrefsWindow::set_connectivity_index);

    QObject::connect(sort_axes, &QCheckBox::clicked,
                     this, &PrefsWindow::set_sort_axes);

    QObject::connect(align_axes, &QCheckBox::clicked,
                     this, &PrefsWindow::set_align_axes);
#ifdef Q_OS_WIN
    menu_style->setEnabled(false);
    menu_style_label->setEnabled(false);
    menu_style->setToolTip(tr("Linux only."));
#endif
}

void PrefsWindow::create_appearance_tab()
{
    appearanceBox = new QGroupBox(this);
    appearanceBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    appearanceBox->setAlignment(Qt::AlignHCenter);

    appearancegrid = new QGridLayout(appearanceBox);
    appearancegrid->setSpacing(20);
    appearancegrid->setContentsMargins(20, 20, 20, 20);

    themeselect = new QComboBox(this);
    themeselect->setToolTip(tr("Change the application appearance.\n"
                               "An Application restart is required,\n"
                               "so that GUI elements appear as intended. "));

#ifdef Q_OS_LINUX
    const auto& keys = QStyleFactory::keys();
    for (const auto& k : keys)
        themeselect->addItem(k.toLower());
#endif

#ifdef Q_OS_WIN
    themeselect->addItems(FusionStyle::getStyles());
    themeselect->setCurrentText(tr("Fusion Zodiac Blue"));
#endif

    themeselect->setCurrentText(QApplication::style()->objectName());

    icon_label = new QLabel(tr("Icon theme"), this);
    iconselect = new QComboBox(this);
    iconselect->setToolTip(tr("Select the icon colour. Auto will determine the icons appearance from the desktop theme."));

    iconselect->addItem(tr("Auto"));
    iconselect->addItem(tr("Black"));
    iconselect->addItem(tr("White"));

    standard_palette = new QCheckBox(tr("Use standard palette."), this);
    standard_palette->setChecked(true);
    standard_palette->setToolTip(tr("In most cases when changing theme this setting is required,\n"
                                    "but not guarenteed. Disable it if necessary.\n"
                                    "An application restart will also be required."));

    icon_size_label = new QLabel(tr("Toolbar icon size"));
    icon_size = new QComboBox(this);
    icon_size->addItem("Small");
    icon_size->addItem("Medium");
    icon_size->addItem("Large");
    icon_size->setCurrentIndex(1);

    icon_size->setToolTip(tr("Change the toolbar icon size."));

    theme_label = new QLabel(tr("Desktop theme"), this);
    theme_label->setToolTip(themeselect->toolTip());

    restore_appearance_defaults = new QPushButton(tr("Defaults"), this);
    restore_appearance_defaults->setToolTip(tr("Restore factory settings."));
    QWidget *spacer = new QWidget(this);

    appearancegrid->addWidget(theme_label, 0, 0, Qt::AlignLeft);
    appearancegrid->addWidget(themeselect, 0, 1);
    appearancegrid->addWidget(standard_palette, 1, 1);
    appearancegrid->addWidget(icon_size_label, 2, 0, Qt::AlignLeft);
    appearancegrid->addWidget(icon_size, 2, 1);
    appearancegrid->addWidget(icon_label, 3, 0);
    appearancegrid->addWidget(iconselect, 3, 1);
    appearancegrid->addWidget(spacer, 4, 1);
    appearancegrid->addWidget(restore_appearance_defaults, 5, 0, Qt::AlignLeft);

    appearanceBox->setLayout(appearancegrid);
}

void PrefsWindow::create_viewport_tab()
{
    viewPortBox = new QGroupBox(this);
    viewPortBox->setAlignment(Qt::AlignHCenter);

    viewPortgrid = new QGridLayout(viewPortBox);
    viewPortgrid->setSpacing(20);
    viewPortgrid->setContentsMargins(20, 20, 20, 20);

    intensity_label = new QLabel(tr("Viewport brightness"), this);
    intensity_label->setToolTip(tr("Adjusts light exposure in the viewport."));

    mouse_speed_label = new QLabel(tr("Key and mouse speed"), this);
    mouse_speed_label->setToolTip(tr("Adjust mouse and keyboard speed responsiveness."));

    viewportcolour = new QComboBox(this);
    viewportcolour->setToolTip(tr("Set the colour of the viewport.\n"
                                  "In some cases this may be set automatically by the Desktop Theme."));

    for (auto iter = SystemSettings::viewport_theme_colours.begin();
         iter != SystemSettings::viewport_theme_colours.end(); ++iter)
        viewportcolour->addItem(iter.key());

    viewportcolour_custom = new ColourButton(this);
    viewportcolour_custom->setToolTip(tr("Set a custom viewport colour."));

    viewport_label = new QLabel(tr("Viewport colour"), this);
    viewport_label->setToolTip(viewportcolour->toolTip());

    intensity = new QSlider(Qt::Horizontal, this);
    intensity->setMinimum(50);
    intensity->setMaximum(150);
    intensity->setValue(100);
    intensity->setSingleStep(5);
    intensity->setPageStep(10);
    intensity->setMinimumWidth(200);
    intensity->setToolTip(tr("Adjust light intensity in the viewport."));

    mouse_speed = new QSlider(Qt::Horizontal, this);
    mouse_speed->setMinimum(25);
    mouse_speed->setMaximum(500);
    mouse_speed->setValue(100);
    mouse_speed->setSingleStep(5);
    mouse_speed->setPageStep(10);
    mouse_speed->setMinimumWidth(200);
    mouse_speed->setToolTip(tr("Adjust mouse and keyboard speed responsiveness."));

    intensity_value = new QLabel(QString::number(intensity->value()) + QString("%"), this);
    restore_viewport_defaults = new QPushButton(tr("Defaults"));
    restore_viewport_defaults->setToolTip(tr("Restore factory settings."));

    QLabel *spacer = new QLabel(this);
    spacer->setEnabled(false);

    viewPortgrid->addWidget(intensity_label, 0, 0, 1, 1, Qt::AlignLeft);
    viewPortgrid->addWidget(intensity, 0, 1, 1, 2, Qt::AlignLeft);
    viewPortgrid->addWidget(intensity_value, 0, 3, 1, 1, Qt::AlignLeft);
    viewPortgrid->addWidget(viewport_label, 1, 0, 1, 1,  Qt::AlignLeft);
    viewPortgrid->addWidget(viewportcolour, 1, 1, 1, 2);
    viewPortgrid->addWidget(viewportcolour_custom, 1, 3);
    viewPortgrid->addWidget(mouse_speed_label, 2, 0, 1, 1, Qt::AlignLeft);
    viewPortgrid->addWidget(mouse_speed, 2, 1, 1, 2, Qt::AlignLeft);
    viewPortgrid->addWidget(spacer, 3, 0, 3, 2);
    viewPortgrid->addWidget(restore_viewport_defaults, 6, 0, Qt::AlignLeft);
    viewPortBox->setLayout(viewPortgrid);
}

void PrefsWindow::create_spectrum_tab()
{
    spectrumBox = new QGroupBox(this);
    spectrumBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    spectrumBox->setAlignment(Qt::AlignHCenter);

    spectrumgrid = new QGridLayout(spectrumBox);
    spectrumgrid->setSpacing(20);
    spectrumgrid->setContentsMargins(20, 20, 20, 20);

    chart_theme_label = new QLabel(tr("Spectrum theme"));
    chart_theme = new QComboBox(this);
    chart_theme->setToolTip(tr("Set a spectrum colour scheme by selecting a theme."));

    spectrum_line_colour = new ColourButton(this);
    spectrum_line_colour->setToolTip(tr("Set a custom spectrum line colour. Auto must be disabled."));
    spectrum_line_colour_label = new QLabel(tr("Spectrum line colour"), this);

    auto_line_colour = new QCheckBox(tr("Auto"), this);
    auto_line_colour->setToolTip(tr("The spectrum line colour will be determined automatically "
                                    "based on theme, when checked."));
    auto_line_colour->setChecked(true);

    spectrum_axes_title_font_label = new QLabel(tr("Axes title font"), this);
    spectrum_axes_title_font = new QPushButton(this);
    spectrum_axes_title_font->setToolTip(tr("The Axes title font to use.\n"
                         "Note that Some fonts don't display well vertically."));

#ifdef Q_OS_LINUX
    spectrum_axes_title_font->setFont(QFont(tr("Nimbus Roman"), 14));
#else
    spectrum_axes_title_font->setFont(QFont(tr("Times New Roman"), 14));
#endif

    spectrum_axes_title_font->setText(spectrum_axes_title_font->font().family() + QString("  ")
                               + QString::number(spectrum_axes_title_font->font().pointSize()));

    spectrum_axes_label_font_label = new QLabel(tr("Axes label font"), this);
    spectrum_axes_label_font = new QPushButton(this);
    spectrum_axes_label_font->setToolTip(tr("The Axes label font to use."));

#ifdef Q_OS_LINUX
    spectrum_axes_label_font->setFont(QFont(tr("Nimbus Roman"), 11));
#else
    spectrum_axes_label_font->setFont(QFont(tr("Times New Roman"), 11));
#endif

    spectrum_axes_label_font->setText(spectrum_axes_label_font->font().family() + QString("  ")
                               + QString::number(spectrum_axes_label_font->font().pointSize()));

    restore_spectrum_defaults = new QPushButton(tr("Defaults"));
    restore_spectrum_defaults->setToolTip(tr("Restore factory settings."));

    spectrum_title_label = new QLabel(tr("Show title"), this);
    spectrum_title = new QCheckBox(this);
    spectrum_title->setToolTip(tr("Whether to show or hide the spectrum title."));
    spectrum_title->setChecked(true);

    chart_theme->addItem("Light", QChart::ChartThemeLight);
    chart_theme->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    chart_theme->addItem("Dark", QChart::ChartThemeDark);
    chart_theme->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    chart_theme->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    chart_theme->addItem("High Contrast", QChart::ChartThemeHighContrast);
    chart_theme->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    chart_theme->addItem("Qt", QChart::ChartThemeQt);
    chart_theme->addItem("Desktop Blend", 8);
    chart_theme->setCurrentIndex(8);

    spectrumgrid->addWidget(chart_theme_label, 0, 0, 1, 1, Qt::AlignLeft);
    spectrumgrid->addWidget(chart_theme, 0, 1, 1, 2);
    spectrumgrid->addWidget(spectrum_line_colour_label, 1, 0, 1, 1, Qt::AlignLeft);
    spectrumgrid->addWidget(auto_line_colour, 1, 1, 1, 1, Qt::AlignLeft);
    spectrumgrid->addWidget(spectrum_line_colour, 1, 2, 1, 1);
    spectrumgrid->addWidget(spectrum_axes_title_font_label, 2, 0, 1, 1, Qt::AlignLeft);
    spectrumgrid->addWidget(spectrum_axes_title_font, 2, 1, 1, 2);
    spectrumgrid->addWidget(spectrum_axes_label_font_label, 3, 0, 1, 1, Qt::AlignLeft);
    spectrumgrid->addWidget(spectrum_axes_label_font, 3, 1, 1, 2);
    spectrumgrid->addWidget(spectrum_title_label, 4, 0, Qt::AlignLeft);
    spectrumgrid->addWidget(spectrum_title, 4, 1, Qt::AlignLeft);
    spectrumgrid->addWidget(restore_spectrum_defaults, 5, 0, Qt::AlignLeft);

    QColor default_colour;
    SystemSettings::theme_is_dark() ? default_colour = QColor(153, 210, 255)
                                    : default_colour = QColor(10, 10, 10);

    spectrum_line_colour->setColour(default_colour);
    spectrum_line_colour->setEnabled(false);

    spectrumBox->setLayout(spectrumgrid);
}

void PrefsWindow::create_system_tab()
{
    systemBox = new QGroupBox(this);
    systemBox->setAlignment(Qt::AlignHCenter);

    systemGrid = new QGridLayout(systemBox);
    systemGrid->setSpacing(20);
    systemGrid->setContentsMargins(20, 20, 20, 20);

    QLabel *desktop_label = new QLabel(tr("Current Desktop"), this);

    current_desktop = new QLabel(this);
    current_desktop->setAlignment(Qt::AlignCenter);
    current_desktop->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    current_desktop->setTextFormat(Qt::RichText);
    current_desktop->setContentsMargins(5, 0, 0, 0);

    const QString desktop = SystemSettings::desktop_environment();

    QSysInfo sysinfo = QSysInfo();
    QString infostr = desktop + "  (" +  sysinfo.prettyProductName().section('(',0,0) + ")";
    if (infostr.length() > 35) infostr.truncate(35);
    current_desktop->setText(infostr);

    menu_style_label = new QLabel(tr("Menu style"), this);
    menu_style = new QComboBox(this);
    menu_style->setToolTip(tr("The Menu style to use. Will be auto detected.\n"
                              "Select a different setting to override."));
    menu_style->insertItem(0, tr("Traditional menu bar"));
    menu_style->setItemData(0, "Offers a menu bar with global menu support.\n"
                               "Suitable for platforms such as KDE.", Qt::ToolTipRole );
    menu_style->insertItem(1, tr("GNOME hamburger menu"));
    menu_style->setItemData(1, "A Hamburger menu located on the top right toolbar.\n"
                               "Suitable for platforms such as GNOME.", Qt::ToolTipRole );

    hide_menu_label = new QLabel(tr("Hide the menu bar"), this);
    hide_menu = new QCheckBox(this);
    hide_menu->setContentsMargins(0, 10, 0, 10);
    hide_menu->setToolTip(tr("Show or hide the menu bar."));

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

    QLabel *dialog_style_label = new QLabel(tr("Use native dialogues"), this);
    dialog_style = new QCheckBox(this);
    dialog_style->setToolTip(tr("Use native or build in dialogue boxes. Non-native dialoguess guarentee that the settings provided by\n"
                                "the application are always honoured, such as file filters, and are therefore recommended."));
    dialog_style->setChecked(false);

    QWidget *spacer = new  QWidget(this);

    restore_system_defaults = new QPushButton(tr("Defaults"), this);
    restore_system_defaults->setToolTip(tr("Restore factory settings (Relevant to the current platform)."));

    systemGrid->addWidget(desktop_label, 0, 0, Qt::AlignLeft);
    systemGrid->addWidget(current_desktop, 0, 1, 1, 2, Qt::AlignLeft);
    systemGrid->addWidget(menu_style_label, 1, 0,  Qt::AlignLeft);
    systemGrid->addWidget(menu_style, 1, 1, 1, 2);
    systemGrid->addWidget(hide_menu_label, 2, 0, Qt::AlignLeft);
    systemGrid->addWidget(hide_menu, 2, 1, Qt::AlignLeft);
    systemGrid->addWidget(dialog_style_label, 3, 0, Qt::AlignLeft);
    systemGrid->addWidget(dialog_style, 3, 1, Qt::AlignLeft);
    systemGrid->addWidget(spacer, 4, 0);
    systemGrid->addWidget(restore_system_defaults, 5, 0, Qt::AlignLeft);

    systemBox->setLayout(systemGrid);

}

void PrefsWindow::create_files_tab()
{
    fileBox = new QGroupBox(this);
    fileBox->setAlignment(Qt::AlignHCenter);

    fileGrid = new QGridLayout(fileBox);
    fileGrid->setSpacing(20);
    fileGrid->setContentsMargins(20, 20, 20, 20);

    connectivity_frame_num = new QComboBox(this);
    connectivity_frame_num->addItem(QStringLiteral("From first frame"));
    connectivity_frame_num->addItem(QStringLiteral("From last frame"));
    connectivity_frame_num->setCurrentIndex(1);
    connectivity_frame_num->setToolTip(tr("The frame number to use for determining bond connectivity.\n"
                                          "(affects xyz files only, cml and normal modes have one frame only)."));

    connectivity_frame_num_label = new QLabel(tr("Molecular connectivity"), this);

    sort_axes = new QCheckBox(tr("Sort the molecule fixed axes\n" "such that Dim X > Dim Y > Dim Z."));
    sort_axes->setChecked(true);
    sort_axes->setToolTip(tr("Allow the molecule to realign such that the molecular\n"
                             "dimensions are Dim X > Dim Y > Dim Z. Affects xyz geometry export."));

    align_axes = new QCheckBox(tr("Align the molecule fixed axes\n" "with the principal axes."));
    align_axes->setChecked(true);
    align_axes->setToolTip(tr("Allow the molecule to realign along the principal axes. Affects xyz\n"
                              "geometry export."));

    QLabel *geom_info_label = new QLabel(this);
   //geom_info_label->setTextFormat(Qt::MarkdownText);
    geom_info_label->setText(tr("The following settings apply on a per file basis, and are not\n"
                                "saved on exit. Changing a setting takes effect the next time\n" "a file is imported or exported only."));

    QLabel *sort_axes_label = new QLabel(tr("Molecule fixed axes"), this);
    QLabel *align_axes_label = new QLabel(tr("Molecule principal axes"), this);
    fileGrid->addWidget(geom_info_label, 0, 0, 2, 3, Qt::AlignHCenter);
    fileGrid->addWidget(connectivity_frame_num_label, 3, 0, Qt::AlignLeft);
    fileGrid->addWidget(connectivity_frame_num, 3, 1, 1, 2);
    fileGrid->addWidget(sort_axes_label, 4, 0,  Qt::AlignLeft);
    fileGrid->addWidget(sort_axes, 4, 1,  Qt::AlignLeft);
    fileGrid->addWidget(align_axes_label, 5, 0,  Qt::AlignLeft);
    fileGrid->addWidget(align_axes, 5, 1,  Qt::AlignLeft);

    fileBox->setLayout(fileGrid);

}

void PrefsWindow::closeEvent(QCloseEvent *evt)
{
    save_settings();
    QWidget::closeEvent(evt);
}

void PrefsWindow::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_Escape)
       hide();

   QWidget::keyPressEvent(event);
}

void PrefsWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::StyleChange)
    {
#ifdef Q_OS_LINUX
       themeselect->blockSignals(true);
       themeselect->setCurrentText(QApplication::style()->objectName().toLower());
       themeselect->blockSignals(false);
       changeViewPort();
#endif
    }

    QWidget::changeEvent(event);
}
