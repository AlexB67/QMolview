#include "mainwindow.h"
#include <QKeySequence>
#include <QShortcut>

MainWindow::MainWindow(QWidget *)
{
    setWindowTitle(QStringLiteral("Molecule Viewer"));

    QWidget *mainwidget = new QWidget(this);
    QGridLayout *grid = new QGridLayout(mainwidget);

    mainwidget->setLayout(grid);
    setCentralWidget(mainwidget);

    numFrames = new QLabel(mainwidget);
    mimeType = new QLabel(mainwidget);

    frameLabel = new QLabel(mainwidget);
    frameLabel->setText(tr("Change Frame"));
    frameLabel->setAlignment(Qt::AlignLeading);

    viewLabel = new QLabel(mainwidget);
    viewLabel->setText(tr("Change View Angle"));
    viewLabel->setAlignment(Qt::AlignLeading);

    x_rotate = new QPushButton(mainwidget);
    x_rotate->setText(tr("Rotate X"));
    x_rotate->setToolTip(tr("Rotate about the molecule X axis."));

    y_rotate = new QPushButton( mainwidget);
    y_rotate->setText(tr("Rotate Y"));
    y_rotate->setToolTip(tr("Rotate about the molecule Y axis."));

    z_rotate = new QPushButton(mainwidget);
    z_rotate->setText(tr("Rotate Z"));
    z_rotate->setToolTip(tr("Rotate about the molecule Z axis."));

    start_mode_display = new QPushButton(set_icon_name(":/icons/media-playback-start-symbolic"), "", mainwidget);
    stop_mode_display = new QPushButton(set_icon_name(":/icons/media-playback-stop-symbolic"), "", mainwidget);

    start_xyz_display = new QPushButton(set_icon_name(":/icons/media-playback-start-symbolic"), "", mainwidget);
    stop_xyz_display = new QPushButton(set_icon_name(":/icons/media-playback-stop-symbolic"), "", mainwidget);

    end_frame = new QPushButton(set_icon_name(":/icons/media-seek-forward-symbolic"), "",  mainwidget);
    start_frame = new QPushButton(set_icon_name(":/icons/media-seek-backward-symbolic"), "", mainwidget);

    start_frame->setToolTip(tr("Jump to the first frame."));
    end_frame->setToolTip(tr("Jump to the last frame."));
    start_mode_display->setToolTip(tr("Play the selected mode."));
    stop_mode_display->setToolTip(tr("Stop playing the selected mode."));
    start_xyz_display->setToolTip(tr("Play all xyz frames (auto repeat)."));
    stop_xyz_display->setToolTip(tr("Stop animate frames."));

    auto_mode_display = new QCheckBox(tr("Auto"), this);
    auto_mode_display->setChecked(false);
    auto_mode_display->setToolTip(tr("When clicking on a row the selected mode will be displayed automatically." ));

    view_angles = new QComboBox(mainwidget);
    view_angles->addItem(tr("Default"));
    view_angles->addItem(tr("Front"));
    view_angles->addItem(tr("Back"));
    view_angles->addItem(tr("Left"));
    view_angles->addItem(tr("Right"));
    view_angles->addItem(tr("Custom"));
    view_angles->setToolTip(tr("Changes the camera view point"));

    frameCount = new QSpinBox(mainwidget);
    frameCount->setRange(1, 1);
    frameCount->setSingleStep(1);
    frameCount->setToolTip(tr("Diplay the selected frame number."));

    r_reset = new QPushButton();
    r_reset->setText(tr("Reset"));
    r_reset->setToolTip(tr("Restore the default view."));

    set_atom_sizes = new QDial(this);
    set_atom_sizes->setRange(10, 135);
    set_atom_sizes->setValue(35);
    set_atom_sizes->setMaximumSize(70, 70);
    set_atom_sizes->setNotchesVisible(true);
    set_atom_sizes->setToolTip(tr("Scales the molecule (atoms and bonds).\n"
                                  "The minimum setting is wireframe, the maximum setting spacefill."));

    atom_size_default = new QPushButton(tr("Default scale"), this);

    scaleAtomsLabel = new QLabel(this);
    scaleAtomsLabel->setText(tr("Scale Atoms"));
    scaleAtomsLabel->setAlignment(Qt::AlignRight);
    scaleAtomsLabel->setToolTip(tr("Scales the molecule (atoms and bonds).\n"
                                   "The minimum is wireframe, maximum is spacefill."));

    normal_modes_table = new QTableWidget(this);
    normal_modes_table->insertColumn(0);
    normal_modes_table->setEditTriggers(QAbstractItemView::NoEditTriggers);


    QStringList header;
    header << tr("𝜔 / cm\u207B\u00B9") << tr("𝐼 / km mol\u207B\u00B9");
    normal_modes_table->setColumnCount(2);
    normal_modes_table->setHorizontalHeaderLabels(header);

#ifdef Q_OS_LINUX
    normal_modes_table->setColumnWidth(0, 140);
    normal_modes_table->setColumnWidth(1, 140);
    normal_modes_table->setMinimumWidth(300);
#endif

#ifdef Q_OS_WIN
    normal_modes_table->setColumnWidth(0, 120);
    normal_modes_table->setColumnWidth(1, 120);
    normal_modes_table->setMinimumWidth(280);
#endif

    normal_modes_table->setSelectionMode(QAbstractItemView::SingleSelection);
    normal_modes_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    normal_modes_table->setToolTip(tr("Select a row to display a normal mode, then press play."));

    // Create 3D window
    view = new MolView3D(this, view_angles);
    set_viewport_colour();

    // create scene
    m_modifier = view->createScene();
    // Put it in a container
    container = view->get_container();
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ctrlBox = new QGroupBox(tr("Manipulate"), this);
    ctrlBox->setAlignment(Qt::AlignHCenter);
    ctrlgrid = new QGridLayout;
    ctrlBox->setLayout(ctrlgrid);
    ctrlBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    ctrlBox->setEnabled(false);

    fileBox = new QGroupBox(this);
    fileBox->setVisible(false);
    fileBox->setAlignment(Qt::AlignHCenter);
    filegrid = new QGridLayout;
    fileBox->setLayout(filegrid);
    fileBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    viewBox = new QGroupBox(tr("View") , this);
    viewBox->setAlignment(Qt::AlignHCenter);
    viewLayout = new QHBoxLayout;
    viewBox->setLayout(viewLayout);
    viewLayout->addWidget(container, 2);

    numFrames->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mimeType->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    filegrid->addWidget(numFrames, 0, 0, 1, 2);
    filegrid->addWidget(mimeType, 1, 0, 1, 2);

    ctrlgrid->addWidget(x_rotate, 0, 0);
    ctrlgrid->addWidget(y_rotate, 0, 1);
    ctrlgrid->addWidget(z_rotate, 1, 0);
    ctrlgrid->addWidget(r_reset, 1, 1);
    //
    ctrlgrid->addWidget(scaleAtomsLabel, 0, 2, Qt::AlignCenter);
    ctrlgrid->addWidget(set_atom_sizes, 1, 2, 2, 1, Qt::AlignCenter);
    ctrlgrid->addWidget(atom_size_default, 3, 2);
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
    ctrlgrid->addWidget(stop_mode_display, 7, 1);
    ctrlgrid->addWidget(auto_mode_display, 7, 2, Qt::AlignHCenter);

    grid->addWidget(viewBox, 0, 0, 4, 4);
    grid->addWidget(ctrlBox, 0, 5);
    grid->addWidget(fileBox, 1, 5);

    ctrlgrid->setContentsMargins(10, 10, 10, 10);
    ctrlgrid->setColumnStretch(0, 2);
    ctrlgrid->setSpacing(10);
    filegrid->setContentsMargins(10, 10, 10, 10);
    filegrid->setSpacing(10);
    viewLayout->setContentsMargins(10, 10, 10, 10);
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
    stop_mode_display->setVisible(false);
    stop_mode_display->setEnabled(false);
    auto_mode_display->setVisible(false);

    start_xyz_display->setVisible(false);
    start_xyz_display->setEnabled(false);
    stop_xyz_display->setVisible(false);
    stop_xyz_display->setEnabled(false);

    setup_molview_signals_and_slots();
    create_menus_and_toolbar();

    // read in settings
    load_settings();
}

void MainWindow::create_menus_and_toolbar()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    editMenu = menuBar()->addMenu(tr("&Edit"));
    helpMenu = menuBar()->addMenu(tr("&Help"));

   // setContextMenuPolicy(Qt::CustomContextMenu);

    openAct = new QAction(set_icon_name(":/icons/document-open-symbolic"), tr("&Open"), this);
    openAct->setToolTip(tr("<qt/>Open a xyz geometry file or a Molden normal  mode file."));
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(openAct->toolTip().section("<qt/>", 1, 1));
    fileMenu->addAction(openAct);

    exitAct = new QAction(set_icon_name(":/icons/application-exit-symbolic"), tr("E&xit"), this);
    exitAct->setToolTip(tr("<qt/>Exit the application"));
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(exitAct->toolTip().section("<qt/>", 1, 1));
    fileMenu->addAction(exitAct);

    aboutAct = new QAction(tr("About &MolView"), this);
    aboutAct->setToolTip(tr("<qt/>Show the About MolView dialog"));
    aboutAct->setStatusTip(aboutAct->toolTip().section("<qt/>", 1, 1));
    helpMenu->addAction(aboutAct);

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setToolTip(tr("<qt/>Show the About Qt box dialog"));
    aboutQtAct->setStatusTip(aboutQtAct->toolTip().section("<qt/>", 1, 1));
    helpMenu->addAction(aboutQtAct);

    prefsAct = new QAction(set_icon_name(":/icons/preferences-system-symbolic"), tr("&Preferences"), this);
    prefsAct->setShortcut(QKeySequence(tr("Ctrl+P")));
    prefsAct->setToolTip(tr("<qt/>Show the preferences dialog"));
    prefsAct->setStatusTip(prefsAct->toolTip().section("<qt/>", 1, 1));
    editMenu->addAction(prefsAct);

    QObject::connect(openAct, &QAction::triggered, this, &MainWindow::openFileDialog);
    QObject::connect(exitAct, &QAction::triggered, this, &MainWindow::close);
    QObject::connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    QObject::connect(aboutQtAct, &QAction::triggered,  qApp, &QApplication::aboutQt);
    QObject::connect(prefsAct, &QAction::triggered,  this, &MainWindow::editPrefs);

    toolbar = addToolBar(tr("File"));
    toolbar->setMovable(true);
    toolbar->setObjectName("File");
    toolbar->addAction(openAct);
    toolbar->addAction(exitAct);

    toolbaredit = addToolBar(tr("Edit"));
    toolbaredit->setMovable(true);
    toolbaredit->setObjectName("Edit");
    toolbaredit->addAction(prefsAct);

    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbaredit->setToolButtonStyle(Qt::ToolButtonIconOnly);

#ifdef Q_OS_MACOS
    setUnifiedTitleAndToolBarOnMac(true);
#endif
}

void MainWindow::play_widget_animations()
{
    // widget animations - quite buggy except for view
    QPropertyAnimation *anim3 = new QPropertyAnimation(viewBox, "geometry", this);
    anim3->setEasingCurve(QEasingCurve::OutBounce);
    anim3->setDuration(1500);
    anim3->setEndValue(viewBox->geometry());
    anim3->setStartValue(QRect(viewBox->x(), viewBox->y() - 200, viewBox->width(), viewBox->height()));


    QParallelAnimationGroup *anim_group = new QParallelAnimationGroup(this);
    anim_group->addAnimation(anim3);
    anim_group->start(QAbstractAnimation::DeleteWhenStopped);
}
