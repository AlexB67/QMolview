#include "appglobals.h"
#include "mainwindow.h"
#include <QKeySequence>
#include <QShortcut>
#include <QtWidgets/QFrame>
#include <QPropertyAnimation>
//#include <QtGui>

MainWindow::MainWindow(QWidget *)
{
#ifdef Q_OS_LINUX
     SystemSettings::systemstyle = QApplication::style()->objectName().toLower();
#endif

#ifdef Q_OS_WIN
    SystemSettings::systemstyle = QApplication::style()->objectName();
    SystemSettings::systemFont = QFont("Segoe UI");
#else
    SystemSettings::systemFont = QApplication::font();
#endif

    setWindowTitle(QStringLiteral("Molecule Viewer"));

    QWidget *mainwidget = new QWidget(this);
    grid = new QGridLayout;

    mainwidget->setLayout(grid);
    setCentralWidget(mainwidget);

    fileinfo_label = new QLabel(mainwidget);
    mimeType = new QLabel(mainwidget);

    frameLabel = new QLabel(mainwidget);
    frameLabel->setText(tr("Change Frame"));
    frameLabel->setAlignment(Qt::AlignLeading);

    viewLabel = new QLabel(mainwidget);
    viewLabel->setText(tr("Change View"));
    viewLabel->setAlignment(Qt::AlignLeading);

    x_rotate = new TextIconButton(tr("X+"), set_icon_name(":/icons/view-refresh-symbolic"), mainwidget);
    x_rotate->setToolTip(tr("Rotate about the X axis."));

    y_rotate = new TextIconButton(tr("Y+"), set_icon_name(":/icons/view-refresh-symbolic"), mainwidget);
    y_rotate->setToolTip(tr("Rotate about the Y axis."));

    z_rotate = new TextIconButton(tr("Z+"), set_icon_name(":/icons/view-refresh-symbolic"), mainwidget);
    z_rotate->setToolTip(tr("Rotate about the Z axis."));

    start_mode_display = new QPushButton(set_icon_name(":/icons/media-playback-start-symbolic"), "", mainwidget);
    pause_mode_display = new QPushButton(set_icon_name(":/icons/media-playback-pause-symbolic"), "", mainwidget);
    stop_mode_display = new QPushButton(set_icon_name(":/icons/media-playback-stop-symbolic"), "", mainwidget);

    start_xyz_display = new QPushButton(set_icon_name(":/icons/media-playback-start-symbolic"), "", mainwidget);
    stop_xyz_display = new QPushButton(set_icon_name(":/icons/media-playback-stop-symbolic"), "", mainwidget);

    end_frame = new QPushButton(set_icon_name(":/icons/media-seek-forward-symbolic"), "",  mainwidget);
    start_frame = new QPushButton(set_icon_name(":/icons/media-seek-backward-symbolic"), "", mainwidget);

    start_frame->setToolTip(tr("Jump to the first frame."));
    end_frame->setToolTip(tr("Jump to the last frame."));
    start_mode_display->setToolTip(tr("Play the selected mode."));

    pause_mode_display->setToolTip(tr("Pause the selected mode.\n"
        "Optionally, use \"Save as\" to write the current\n"
        "displaced geometry to a file (in xyz format)."));

    stop_mode_display->setToolTip(tr("Stop playing the selected mode."));
    start_xyz_display->setToolTip(tr("Play all xyz frames (auto repeat)."));
    stop_xyz_display->setToolTip(tr("Stop animate frames."));

    auto_mode_display = new QCheckBox(this);
    auto_mode_display->setIcon(set_icon_name(":/icons/view-continuous-symbolic"));
    auto_mode_display->setLayoutDirection(Qt::RightToLeft);
    auto_mode_display->setShortcut(QKeySequence(tr("Ctrl+Y")));
    auto_mode_display->setChecked(true);
    auto_mode_display->setToolTip(tr("When checked, clicking on a row will animate\n"
                                     "the selected normal mode automatically." ));

    show_axes = new QCheckBox(this);
    show_axes->setIcon(set_icon_name(":/icons/axis-symbolic"));
    show_axes->setTristate(true);
    show_axes->setLayoutDirection(Qt::RightToLeft);
    show_axes->setShortcut(QKeySequence(tr("Ctrl+X")));
    show_axes->setChecked(false);
    show_axes->setToolTip(tr("Partial check: Show the molecule fixed axes.\n"
                             "Full Check: Show the molecule fixed axes with transparency.\n"
                             "Uncheck: Hide axes and disable transparency."));

    show_norm_vectors = new QCheckBox(this);
    show_norm_vectors->setTristate(true);
    show_norm_vectors->setIcon(set_icon_name(":/icons/vector-symbolic"));
    show_norm_vectors->setLayoutDirection(Qt::RightToLeft);
    show_norm_vectors->setShortcut(QKeySequence(tr("Ctrl+N")));
    show_norm_vectors->setChecked(false);
    show_norm_vectors->setToolTip(tr("Partial check: Show normal mode vectors.\n"
                                     "Full Check: Show normal mode vectors with transparency.\n"
                                     "Uncheck: Hide vectors and disable transparency."));

    view_angles = new QComboBox(mainwidget);
    view_angles->addItem(tr("Perspective 1"));
    view_angles->addItem(tr("Perspective 2"));
    view_angles->addItem(tr("Perspective 3"));
    view_angles->addItem(tr("Front"));
    view_angles->addItem(tr("Back"));
    view_angles->addItem(tr("Left"));
    view_angles->addItem(tr("Right"));
    view_angles->addItem(tr("Top"));
    view_angles->addItem(tr("Bottom"));
    view_angles->setToolTip(tr("Changes the camera view point"));

    // custom placeholder for ComboBox
    placeholderComboText = new QLabel(tr("Custom"), this);
    view_angles->setLayout(new QVBoxLayout(view_angles));
    view_angles->layout()->setContentsMargins(5, 0, 0, 0);
    view_angles->layout()->addWidget(placeholderComboText);
    placeholderComboText->setVisible(false);

    frameCount = new QSpinBox(mainwidget);
    frameCount->setRange(1, 1);
    frameCount->setSingleStep(1);
    frameCount->setToolTip(tr("Diplay the selected frame number."));

    r_reset = new QPushButton();
    r_reset->setText(tr("Reset"));
    r_reset->setShortcut(QKeySequence(tr("Ctrl+R")));
    r_reset->setToolTip(tr("Restore the default view."));

    snap_to_centre = new QCheckBox(tr("Snap"), this);
    snap_to_centre->setChecked(true);
    snap_to_centre->setToolTip(tr("When checked, using the rotate buttons\n"
                                  "will snap the molecule onto the origin."));

    set_atom_sizes = new QDial(this);
    set_atom_sizes->setRange(10, 135);
    set_atom_sizes->setValue(35);
    set_atom_sizes->setMaximumSize(70, 70);
    set_atom_sizes->setNotchesVisible(true);
    set_atom_sizes->setToolTip(tr("Scales the model (atoms and bonds). The minimum setting is wireframe.\n"
                                  "The maximum setting spacefill. Adjust for anything in between."));

    scaleAtomsLabel = new QLabel(tr("Scale Atoms"), this);
    scaleAtomsLabel->setAlignment(Qt::AlignHCenter);
    scaleAtomsLabel->setToolTip(set_atom_sizes->toolTip());

    animation_speed_label = new QLabel(tr("Speed"), this);
    animation_speed_label->setAlignment(Qt::AlignLeft);

    amplitude_label = new QLabel(tr("Amplitude"), this);
    amplitude_label->setAlignment(Qt::AlignLeft);

    animation_speed = new Slider(this);
    animation_speed->setTickPosition(QSlider::TicksAbove);
    animation_speed->setRange(0, 130);
    animation_speed->setValue(67);
    animation_speed->setSingleStep(2);
    animation_speed->setPageStep(10);
    animation_speed->setToolTip(tr("Increase or decrease the animation speed"));

    amplitude = new Slider(this);
    amplitude->setTickPosition(QSlider::TicksAbove);
    amplitude->setRange(10, 140);
    amplitude->setValue(75);
    amplitude->setSingleStep(1);
    amplitude->setToolTip(tr("Increase or decrease the normal mode amplitude scaling."));

    animinfo_label = new QLabel(this);
    animinfo_label->setAlignment(Qt::AlignCenter);

    caminfo_label = new QLabel(tr("Pan Mode: Y"), this);
    caminfo_label ->setAlignment(Qt::AlignCenter);
    caminfo_label->setToolTip(tr("Hold and move the right mouse to tilt and pan around the Y axis.\n"
                                 "Hold the Alt key to switch panning around the Z axis.\n"
                                 "Hold the left mouse and move to change view point."));

    normal_modes_table = new QTableWidget(this);
    normal_modes_table->insertColumn(0);
    normal_modes_table->setEditTriggers(QAbstractItemView::NoEditTriggers);


    QStringList header;
    header << tr("𝜔 / cm\u207B\u00B9") << tr("𝐼 / km mol\u207B\u00B9");
    normal_modes_table->setColumnCount(2);
    normal_modes_table->setHorizontalHeaderLabels(header);

    normal_modes_table->setColumnWidth(0, 130);
    normal_modes_table->setColumnWidth(1, 145);
    normal_modes_table->setMinimumWidth(300);
    normal_modes_table->setSelectionMode(QAbstractItemView::SingleSelection);
    normal_modes_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    normal_modes_table->setToolTip(tr("Select a row to animate a normal mode."));

    // Create 3D window
    view = new MolView3D(this, view_angles, caminfo_label);
    container = QWidget::createWindowContainer(view, this);
    // Put it in a container
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QSize screenSize = screen()->size();
    container->setMinimumSize(QSize(560, 512));
    container->setMaximumSize(screenSize);
    // create scene
    m_modifier = view->createScene();

    ctrlBox = new QGroupBox(tr("Manipulate"), this);
    ctrlBox->setAlignment(Qt::AlignHCenter);
    ctrlgrid = new QGridLayout;
    ctrlgrid->setColumnStretch(0, 1);
    ctrlgrid->setColumnStretch(1, 1);
    ctrlgrid->setColumnStretch(2, 1);
    ctrlgrid->setContentsMargins(10, 10, 10, 10);
    ctrlgrid->setSpacing(10);

    ctrlBox->setLayout(ctrlgrid);
    ctrlBox->setMaximumWidth(350);
    ctrlBox->setEnabled(false);

    viewBox = new QGroupBox(tr("View"), this);
    viewBox->setContentsMargins(5, 5, 5, 0);
    viewBox->setAlignment(Qt::AlignHCenter);
    viewLayout = new QHBoxLayout;
    viewBox->setLayout(viewLayout);
    viewLayout->addWidget(container, 2);

#ifdef Q_OS_LINUX
    viewLayout->setContentsMargins(5, 0, 5, 5);
#else
    viewLayout->setContentsMargins(10, 10, 10, 10);
#endif

    ctrlgrid->addWidget(x_rotate, 0, 0);
    ctrlgrid->addWidget(y_rotate, 0, 1);
    ctrlgrid->addWidget(z_rotate, 0, 2);
    ctrlgrid->addWidget(r_reset, 1, 0);
    ctrlgrid->addWidget(snap_to_centre, 1, 1, Qt::AlignCenter);

    //
    ctrlgrid->addWidget(scaleAtomsLabel, 1, 2, Qt::AlignCenter);
    ctrlgrid->addWidget(set_atom_sizes, 2, 2, 2, 1, Qt::AlignCenter);
    //
    ctrlgrid->addWidget(viewLabel, 2, 0, 1, 2, Qt::AlignHCenter);
    ctrlgrid->addWidget(view_angles, 3, 0, 1, 2);
    ctrlgrid->addWidget(frameLabel, 4, 0);
    ctrlgrid->addWidget(frameCount, 4, 1);
    ctrlgrid->addWidget(start_xyz_display, 4, 2);
    ctrlgrid->addWidget(start_frame, 5, 0);
    ctrlgrid->addWidget(end_frame, 5, 1);
    ctrlgrid->addWidget(stop_xyz_display, 5, 2);
    ctrlgrid->addWidget(normal_modes_table, 6, 0, 1, 3);
    ctrlgrid->addWidget(start_mode_display, 7, 0);
    ctrlgrid->addWidget(pause_mode_display, 7, 1);
    ctrlgrid->addWidget(stop_mode_display, 7, 2);
    ctrlgrid->addWidget(animation_speed_label, 8, 0, 1, 1);
    ctrlgrid->addWidget(animation_speed, 8, 1, 1, 2);
    ctrlgrid->addWidget(amplitude_label, 9, 0, 1, 1);
    ctrlgrid->addWidget(amplitude, 9, 1, 1, 2);

    ctrlgrid->setColumnMinimumWidth(0, 100);
    ctrlgrid->setColumnMinimumWidth(1, 100);
    ctrlgrid->setColumnMinimumWidth(2, 100);

    grid->addWidget(viewBox, 0, 0, 4, 4);
    grid->addWidget(ctrlBox, 0, 5);

    grid->setContentsMargins(10, 10, 10, 10);
    grid->setSpacing(10);

    // startup conditions
    frameCount->setVisible(false);
    frameLabel->setVisible(false);
    start_frame->setVisible(false);
    end_frame->setVisible(false);
    normal_modes_table->setVisible(false);
    start_mode_display->setVisible(false);
    start_mode_display->setEnabled(false);
    pause_mode_display->setVisible(false);
    pause_mode_display->setEnabled(false);
    stop_mode_display->setVisible(false);
    stop_mode_display->setEnabled(false);
    auto_mode_display->setEnabled(false);
    auto_mode_display->setChecked(true);
    animation_speed_label->setVisible(false);
    animation_speed->setVisible(false);
    amplitude_label->setVisible(false);
    amplitude->setVisible(false);

    start_xyz_display->setVisible(false);
    start_xyz_display->setEnabled(false);
    stop_xyz_display->setVisible(false);
    stop_xyz_display->setEnabled(false);
    viewBox->setEnabled(false);

    setup_molview_signals_and_slots();
    create_menus_and_toolbar();

    auto font = this->font();

 #ifdef Q_OS_LINUX
     font.setPointSizeF(font.pointSizeF() * 0.9);
     statusBar()->setContentsMargins(10, 0, 0, 1);
 #endif

 #ifdef Q_OS_WIN
     font.setPointSizeF(font.pointSizeF() * 0.98);
     statusBar()->setContentsMargins(10, 0, 10, 2);
     statusBar()->setSizeGripEnabled(false);
 #endif

    fileinfo_label->setTextFormat(Qt::MarkdownText);
    fileinfo_label->setFont(font);
    caminfo_label->setFont(font);
    animinfo_label->setFont(font);
    mimeType->setFont(font);

    statusBar()->addPermanentWidget(fileinfo_label, 2);
    statusBar()->addPermanentWidget(caminfo_label, 2);
    statusBar()->addPermanentWidget(animinfo_label, 2);
    statusBar()->addPermanentWidget(mimeType, 0);

#ifdef Q_OS_WIN
    statusBar()->setSizeGripEnabled(false);
#endif

    caminfo_label->setVisible(false);
    set_viewport_colour(); // if user qsettings are available will override later
    // read in settings
    load_settings();
}

void MainWindow::create_menus_and_toolbar()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    editMenu = menuBar()->addMenu(tr("&Edit"));
    viewMenu = menuBar()->addMenu(tr("&View"));
    helpMenu = menuBar()->addMenu(tr("&Help"));

   // setContextMenuPolicy(Qt::CustomContextMenu);

    openAct = new QAction(set_icon_name(":/icons/document-open-symbolic"), tr("&Open"), this);
    openAct->setToolTip(tr("Open a xyz geometry file or a Molden normal  mode file."));
    openAct->setIconVisibleInMenu(true);
    openAct->setShortcuts(QKeySequence::Open);
    fileMenu->addAction(openAct);

    saveAsAct = new QAction(set_icon_name(":/icons/document-save-as-symbolic"), tr("Sa&ve as"), this);
    saveAsAct->setToolTip(tr("Save the molecule as an xyz file."));
    saveAsAct->setIconVisibleInMenu(true);
    saveAsAct->setShortcut(QKeySequence(tr("Ctrl+W")));
    fileMenu->addAction(saveAsAct);

    exitAct = new QAction(set_icon_name(":/icons/application-exit-symbolic"), tr("E&xit"), this);
    exitAct->setToolTip(tr("Exit the application"));
    exitAct->setIconVisibleInMenu(true);
    exitAct->setShortcuts(QKeySequence::Quit);
    fileMenu->addAction(exitAct);

    aboutAct = new QAction(set_icon_name(":/icons/about-symbolic"), tr("About QMolView"), this);
    aboutAct->setToolTip(tr("Show the About MolView dialog"));
    aboutAct->setIconVisibleInMenu(true);
    helpMenu->addAction(aboutAct);

    aboutQtAct = new QAction(set_icon_name(":/icons/about-symbolic"), tr("About &Qt"), this);
    aboutQtAct->setToolTip(tr("Show the About Qt box dialog"));
    aboutQtAct->setIconVisibleInMenu(true);
    helpMenu->addAction(aboutQtAct);

    helpAct = new QAction(set_icon_name(":/icons/help-browser-symbolic"), tr("Hel&p"), this);
    helpAct->setToolTip(tr("The QMolView Manual"));
    helpAct->setShortcut(QKeySequence(tr("F1")));
    helpAct->setIconVisibleInMenu(true);
    helpMenu->addAction(helpAct);

    prefsAct = new QAction(set_icon_name(":/icons/preferences-system-symbolic"), tr("&Preferences"), this);
    prefsAct->setShortcut(QKeySequence(tr("Ctrl+P")));
    prefsAct->setIconVisibleInMenu(true);
    prefsAct->setToolTip(tr("Show the preferences dialog"));
    editMenu->addAction(prefsAct);

    shortcutsAct = new QAction(set_icon_name(":/icons/keyboard-shortcuts-symbolic"), tr("Shortcuts"), this);
    shortcutsAct->setShortcut(QKeySequence(tr("Ctrl+K")));
    shortcutsAct->setIconVisibleInMenu(true);
    shortcutsAct->setToolTip(tr("Show the keyboard, mouse and shortcuts settings window."));
    helpMenu->addAction(shortcutsAct);

    viewSpectrumAct = new QAction(set_icon_name(":/icons/video-display-symbolic"), tr("View &Spectrum"), this);
    viewSpectrumAct->setShortcut(QKeySequence(tr("Ctrl+S")));
    viewSpectrumAct->setIconVisibleInMenu(true);
    viewSpectrumAct->setToolTip(tr("Show the Spectrum window, when a spectrum is available."));
    viewMenu->addAction(viewSpectrumAct);

    rotationModeAct = new QAction(set_icon_name(":/icons/view-refresh-symbolic"), tr(""), this);
    rotationModeAct->setShortcut(QKeySequence(tr("Ctrl+M")));
    rotationModeAct->setCheckable(true);
    rotationModeAct->setToolTip(tr("Toggle the rotate buttons from interactive to animation mode."));

    rotateleftAct = new QAction(set_icon_name(":/icons/object-rotate-left-symbolic"), tr(""), this);
    rotateleftAct->setShortcut(QKeySequence(tr("-")));
    rotateleftAct->setToolTip(tr("Rotate buttons: Enable anti-clockwise rotation."));

    rotaterightAct = new QAction(set_icon_name(":/icons/object-rotate-right-symbolic"), tr(""), this);
    rotaterightAct->setShortcut(QKeySequence(tr("=")));
    rotaterightAct->setToolTip(tr("Rotate buttons: Enable clockwise rotation."));

    atomsScaleAct  = new QAction(set_icon_name(":/icons/view-restore-symbolic"), tr(""), this);
    atomsScaleAct->setShortcut(QKeySequence(tr("Ctrl+A")));
    atomsScaleAct->setToolTip(tr("Restore default atom and bond scaling."));

    defaultAnimAct = new QAction(set_icon_name(":/icons/emblem-default-symbolic"), tr(""), this);
    defaultAnimAct->setShortcut(QKeySequence(tr("Ctrl+D")));
    defaultAnimAct ->setToolTip(tr("Restore default animation settings."));

    QObject::connect(openAct, &QAction::triggered, this, &MainWindow::openFileDialog);
    QObject::connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAsxyz);
    QObject::connect(exitAct, &QAction::triggered, this, &MainWindow::close);
    QObject::connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    QObject::connect(aboutQtAct, &QAction::triggered,  qApp, &QApplication::aboutQt);
    QObject::connect(helpAct, &QAction::triggered, this, &MainWindow::launchHelp);
    QObject::connect(prefsAct, &QAction::triggered,  this, &MainWindow::editPrefs);
    QObject::connect(shortcutsAct, &QAction::triggered,  this, &MainWindow::showShortcuts);
    QObject::connect(viewSpectrumAct, &QAction::triggered,  this, &MainWindow::showSpectrum);
    QObject::connect(rotateleftAct, &QAction::triggered, this, &MainWindow::rotateLeft);
    QObject::connect(rotaterightAct, &QAction::triggered, this, &MainWindow::rotateRight);
    QObject::connect(rotationModeAct, &QAction::triggered, this, &MainWindow::setRotationMode);
    QObject::connect(defaultAnimAct, &QAction::triggered, this, &MainWindow::setDefaultAnimationParams);
    QObject::connect(atomsScaleAct, &QAction::triggered, this, &MainWindow::setDefaultAtomScale);

    toolbar = addToolBar(tr("File"));
    toolbar->setMovable(true);
    toolbar->setObjectName("File");
    toolbar->addAction(openAct);
    toolbar->addAction(saveAsAct);
    toolbar->addAction(exitAct);

    toolbaredit = addToolBar(tr("Edit"));
    toolbaredit->setMovable(true);
    toolbaredit->setObjectName("Edit");
    toolbaredit->addAction(prefsAct);

    toolbarhelp = addToolBar(tr("Help"));
    toolbarhelp->setMovable(true);
    toolbarhelp->setObjectName("Help");
    toolbarhelp->addAction(shortcutsAct);

    toolbarview = addToolBar(tr("View"));
    toolbarview->setMovable(true);
    toolbarview->setObjectName("View");
    toolbarview->addAction(viewSpectrumAct);

    toolbarctrl = addToolBar(tr("Control"));
    toolbarctrl->setMovable(true);
    toolbarctrl->setObjectName("View");
    toolbarctrl->addAction(rotationModeAct);
    toolbarctrl->addAction(rotateleftAct);
    toolbarctrl->addAction(rotaterightAct);
    toolbarctrl->addAction(atomsScaleAct);
    toolbarctrl->addAction(defaultAnimAct);
    toolbarctrl->addSeparator();
    toolbarctrl->addWidget(auto_mode_display);
    toolbarctrl->addSeparator();
    toolbarctrl->addWidget(show_axes);
    toolbarctrl->addSeparator();
    toolbarctrl->addWidget(show_norm_vectors);

    toolbarmenu = addToolBar(tr("Menu"));
    toolbarmenu->setIconSize(QSize(16, 16));
    toolbarmenu->setMovable(false);

    hamBurgerMenu = new QMenu(this);
    hamBurgerMenu->addAction(openAct);
    hamBurgerMenu->addAction(saveAsAct);
    hamBurgerMenu->addSeparator();
    hamBurgerMenu->addAction(viewSpectrumAct);
    hamBurgerMenu->addSeparator();
    hamBurgerMenu->addAction(prefsAct);
    hamBurgerMenu->addSeparator();
    hamBurgerMenu->addAction(shortcutsAct);
    hamBurgerMenu->addAction(helpAct);
    hamBurgerMenu->addSeparator();
    hamBurgerMenu->addAction(aboutAct);
    hamBurgerMenu->addAction(aboutQtAct);
    hamBurgerMenu->addSeparator();
    hamBurgerMenu->addAction(exitAct);
    hamBurgerMenu->installEventFilter(this);
    hamBurgerMenu->setAttribute(Qt::WA_TranslucentBackground);
    hamBurgerMenu->setStyleSheet(get_menu_style_sheet());

    toolButton = new QToolButton(this);
    toolButton->setStyleSheet("QToolButton::menu-indicator{image:none;}");
    toolButton->setMenu(hamBurgerMenu);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    toolButton->setAutoRaise(false);
    toolButton->setIcon(set_icon_name(":/icons/open-menu-symbolic"));
    toolButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    QWidgetAction* toolButtonAction = new QWidgetAction(this);
    toolButtonAction->setDefaultWidget(toolButton);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbarmenu->addWidget(spacer);
    toolbarmenu->addAction(toolButtonAction);
    toolbarmenu->setContentsMargins(0, 0, 5, 0);


    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbaredit->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbarhelp->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbarview->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbarctrl->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbarmenu->setToolButtonStyle(Qt::ToolButtonIconOnly);

    toolbarctrl->setEnabled(false);
    rotaterightAct->setEnabled(false);
    defaultAnimAct->setEnabled(false);
    atomsScaleAct->setEnabled(false);
    saveAsAct->setEnabled(false);
    viewSpectrumAct->setEnabled(false);

    icon_size_changed(1);

#ifdef Q_OS_MACOS
    setUnifiedTitleAndToolBarOnMac(true);
#endif

    const QString desktop = SystemSettings::desktop_environment();
    if (desktop.contains("gnome", Qt::CaseInsensitive))
        set_menubar_style(1);
    else
        set_menubar_style(0);


    showSpectrum();
    spectrumwindow->hide();
}

void MainWindow::play_widget_animations()
{
    // widget animations - quite buggy except for view
    QPropertyAnimation *anim3 = new QPropertyAnimation(viewBox, "geometry", this);
    anim3->setEasingCurve(QEasingCurve::OutBounce);
    anim3->setDuration(1500);
    anim3->setEndValue(viewBox->geometry());
    anim3->setStartValue(QRect(viewBox->x(), viewBox->y() - 200, viewBox->width(), viewBox->height()));


    //QParallelAnimationGroup *anim_group = new QParallelAnimationGroup(this);
    //anim_group->addAnimation(anim3);
    anim3->start(QAbstractAnimation::DeleteWhenStopped);
}

bool MainWindow::eventFilter(QObject * obj, QEvent *event)
{
    if (event->type() == QEvent::Show && obj == toolButton->menu())
    {
        QPoint pos = toolButton->menu()->pos();
        auto width = toolButton->menu()->rect().width() + toolButton->menu()->contentsMargins().right();
        toolButton->menu()->move(this->geometry().topRight().x() + ctrlBox->contentsMargins().right() - width, pos.y());

        return true;
    }

    return QMainWindow::eventFilter(obj, event);
}

QString MainWindow::get_menu_style_sheet()
{
    auto p = qApp->palette();
    auto colour = p.color(QPalette::Highlight);
    QString col = QString("rgb(") + QString::number(colour.red()) + "," +
    QString::number(colour.green()) + "," + QString::number(colour.blue()) + ")";

    auto bg_colour = p.color(QPalette::AlternateBase);
    QString bg_col = QString("rgb(") + QString::number(bg_colour.red()) + "," +
    QString::number(bg_colour.green()) + "," + QString::number(bg_colour.blue()) + ")";

    if (SystemSettings::theme_is_dark())
    {
        return("QMenu{background-color: " + bg_col +  "; border: 2px solid #272727; "
               "border-radius: 10px; margin: 5px; }"
               "QMenu::item { border-radius: 5px; padding: 3px 3px; margin: 3px 10px;}"
               "QMenu::item:selected {  background-color:" + col + "; }"
               "QMenu::separator {margin-left: 25px; margin-right: 25px;}");
    }
    else
        return("QMenu{background-color: " + bg_col +  "; border: 1px solid #C0C0C0; "
               "border-radius: 10px; margin: 5px; }"
               "QMenu::item { border-radius: 5px; padding: 3px 3px; margin: 3px 10px;}"
               "QMenu::item:selected {  background-color:" + col + "; }"
               "QMenu::separator {background: #D3D3D3; height: 1px; margin-left: 25px; margin-right: 25px;}");
}

void MainWindow::changeEvent(QEvent* event)
{
#ifdef Q_OS_LINUX
    static bool is_start_up{true};
#endif

    if (event->type() == QEvent::StyleChange)
    {
        icon_theme_changed();

    qApp->setFont(QFont(SystemSettings::systemFont));
    menuBar()->setFont(SystemSettings::systemFont);

#ifdef Q_OS_LINUX
        hamBurgerMenu->setStyleSheet(get_menu_style_sheet());
        hamBurgerMenu->update();

        if (!prefswindow && is_start_up)
        {
            set_viewport_colour();
            is_start_up = false;
        }
#endif
    }

    QMainWindow::changeEvent(event);
}

