#include "prefswindow.h"

#ifdef Q_OS_WIN
#include "fusionstyles.h"
#endif

#include <QtWidgets/QApplication>
#include <QPalette>
#include <QStyleFactory>
#include <QKeyEvent>

PrefsWindow::PrefsWindow(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle(tr("Preferences"));

    appearanceBox = new QGroupBox(tr("Appearance"), this);
    appearanceBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    appearanceBox->setAlignment(Qt::AlignHCenter);
    appearancegrid = new QGridLayout(appearanceBox);
    appearancegrid->setSpacing(20);
    appearancegrid->setContentsMargins(20, 20, 20, 20);

//    miscBox = new QGroupBox(tr("Miscellaneous"), this);
//    miscBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
//    miscBox->setAlignment(Qt::AlignHCenter);
//    miscgrid = new QGridLayout(miscBox);
//    miscgrid->setSpacing(20);
//    miscgrid->setMargin(20);

    grid = new QGridLayout(this);
    grid->setContentsMargins(10, 10, 10, 10);

    themeselect = new QComboBox(this);
    themeselect->setToolTip(tr("Change the appearance of the application."));

#ifdef Q_OS_LINUX
    const auto& keys = QStyleFactory::keys();
    for (const auto& k : keys)
        themeselect->addItem(k.toLower());
#endif

#ifdef Q_OS_WIN
    themeselect->addItems(FusionStyle::getStyles());
#endif

    themeselect->setCurrentText(QApplication::style()->objectName());

    viewportcolour = new QComboBox(this);
    viewportcolour->setToolTip(tr("Set the colour of the view port."));
    viewportcolour->addItem(tr("Default"));
    viewportcolour->addItem(tr("Adwaita"));
    viewportcolour->addItem(tr("Adwaita-dark"));
    viewportcolour->addItem(tr("Black"));
    viewportcolour->addItem(tr("Breeze"));
    viewportcolour->addItem(tr("Fusion"));
    viewportcolour->addItem(tr("Layan"));

    icon_label = new QLabel(tr("Icons set"), this);
    iconselect = new QComboBox(this);
    iconselect->setToolTip(tr("Select the icon colour. Auto will determine the icons appearance from the desktop theme."));

    iconselect->addItem(tr("Auto"));
    iconselect->addItem(tr("Black"));
    iconselect->addItem(tr("White"));

    standard_palette = new QCheckBox(tr("Use standard palette."), this);
    standard_palette->setChecked(true);
    standard_palette->setToolTip(tr("In most cases when changing theme this setting is required, but not guarenteed.\n"\
                                    "Disable it if necessary."
                                    " An application restart may also be required."));

    hide_menu = new QCheckBox(tr("Hide the Menu bar."), this);
    hide_menu->setToolTip(tr("Show or hide the Menu bar."));

    theme_label = new QLabel(tr("Select theme"), this);
    theme_label->setToolTip(themeselect->toolTip());
    viewport_label = new QLabel(tr("Viewport colour"), this);
    viewport_label->setToolTip(viewportcolour->toolTip());

    restore_defaults = new QPushButton(tr("Defaults"), this);
    restore_defaults->setToolTip(tr("Restore factory defaults."));

    appearancegrid->addWidget(theme_label, 0, 0, 1, 1, Qt::AlignHCenter);
    appearancegrid->addWidget(themeselect, 1, 0, 1, 1);
    appearancegrid->addWidget(viewport_label, 2, 0, 1, 1,  Qt::AlignHCenter);
    appearancegrid->addWidget(viewportcolour, 3, 0, 1, 1);
    appearancegrid->addWidget(hide_menu, 0, 1, 1, 1);
    appearancegrid->addWidget(standard_palette, 1, 1, 1, 1);
    appearancegrid->addWidget(icon_label, 2, 1, 1, 1, Qt::AlignHCenter);
    appearancegrid->addWidget(iconselect, 3, 1, 1, 1);
    appearancegrid->addWidget(restore_defaults, 4, 1, 1, 1);


    appearanceBox->setLayout(appearancegrid);
   // miscBox->setLayout(miscgrid);
    grid->addWidget(appearanceBox, 0, 0);
    //grid->addWidget(miscBox, 1, 0); //might need it later
    setLayout(grid);

    load_settings(); // before we make any connections

    QObject::connect(themeselect, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &PrefsWindow::changeTheme);

#ifdef Q_OS_LINUX
    QObject::connect(viewportcolour, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &PrefsWindow::changeViewPort);
#endif

    QObject::connect(hide_menu, &QCheckBox::clicked,
                     this, &PrefsWindow::changeMenu);

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

    QObject::connect(restore_defaults, &QPushButton::clicked,
                     this, &PrefsWindow::removeSettings);
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
