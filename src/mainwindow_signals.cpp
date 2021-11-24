#include "mainwindow.h"
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMimeDatabase>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTableWidgetItem>
#include <QSysInfo>
#include <QTimer>


void MainWindow::sendX()
{
    m_modifier->RotateX(x_rotate, view_angles);
}

void MainWindow::sendY()
{
    m_modifier->RotateY(y_rotate, view_angles);
}

void MainWindow::sendZ()
{
    m_modifier->RotateZ(z_rotate, view_angles);
}

void MainWindow::sendAtomSizes()
{
    m_modifier->scale_atoms(set_atom_sizes);
}

void MainWindow::sendFrameIndex()
{
    if (frameCount->value() == frameCount->maximum())
    {
        end_frame->setEnabled(false);
        start_frame->setEnabled(true);
    }
    else if (frameCount->value() == frameCount->minimum())
    {
        end_frame->setEnabled(true);
        start_frame->setEnabled(false);
    }
    else
    {
        end_frame->setEnabled(true);
        start_frame->setEnabled(true);
    }

    m_modifier->showFrame(frameCount);
}

void MainWindow::sendsetFirstFrame()
{
    stopAnimation();
    start_frame->setEnabled(false);
    end_frame->setEnabled(true);
    frameCount->setValue(1);
}

void MainWindow::sendsetLastFrame()
{
    stopAnimation();
    start_frame->setEnabled(true);
    end_frame->setEnabled(false);
    frameCount->setValue(frameCount->maximum());
}

void MainWindow::sendResetView()
{
   m_modifier->resetView(view_angles);
   m_modifier->reset();
}

void MainWindow::sendplayNormalMode()
{   // start normal playing start button
    start_mode_display->setEnabled(false);
    stop_mode_display->setEnabled(true);

    if (play_mode_timer)
    {
        if(!play_mode_timer->isActive())
            play_mode_timer->start(25);
    }
}

void MainWindow::sendstopNormalMode()
{   // Stop normal playing stop button
    start_mode_display->setEnabled(true);
    stop_mode_display->setEnabled(false);

    if (play_mode_timer)
        play_mode_timer->stop();

    m_modifier->stop_normal_mode();
}

void MainWindow::activatePlay()
{   // Stop normal playing when clicking on a table row
    start_mode_display->setEnabled(true);
    stop_mode_display->setEnabled(false);

    if (play_mode_timer)
        play_mode_timer->stop();

    m_modifier->stop_normal_mode();

    if (auto_mode_display->isChecked())
        sendplayNormalMode();
}

void MainWindow::openFileDialog()
{

    if (play_mode_timer) play_mode_timer->stop();
    if (play_xyz_timer) play_xyz_timer->stop();

    QFileDialog dialog;
    dialog.setModal(true);
    QString file = dialog.getOpenFileName(this, tr("Open a molecule xyz or Normal Mode Molden file"),
                                             QDir::homePath(), tr("*.xyz *.molden_normal_modes"));
    QFileInfo filext(file);
    if (!file.length())
        return;
    else if (filext.suffix() != tr("xyz") && filext.suffix() != tr("molden_normal_modes"))
        return;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(file);
    mimeType->setText(QString(tr("Mime type: ")) + type.name());
#endif

    m_modifier->openFile(file, filext.suffix());

    fileBox->setVisible(true);
    fileBox->setTitle(tr("File Information"));
    ctrlBox->setEnabled(true);
    frameCount->setRange(1, static_cast<int>(m_modifier->get_frames_count()));
    frameCount->setValue(1);

    if (m_modifier->get_frames_count() > 1)
    {
        frameCount->setVisible(true);
        frameLabel->setVisible(true);
        start_frame->setVisible(true);
        start_frame->setEnabled(false);
        end_frame->setVisible(true);
        start_xyz_display->setVisible(true);
        start_xyz_display->setEnabled(true);
        stop_xyz_display->setVisible(true);
        stop_xyz_display->setEnabled(false);
    }
    else
    {
        frameCount->setVisible(false);
        frameLabel->setVisible(false);
        start_frame->setVisible(false);
        end_frame->setVisible(false);
        start_xyz_display->setVisible(false);
        stop_xyz_display->setVisible(false);
    }

    if (filext.suffix() != "xyz")
    {
        normal_modes_table->setVisible(true);
        start_mode_display->setVisible(true);
        start_mode_display->setEnabled(false);
        stop_mode_display->setVisible(true);
        stop_mode_display->setEnabled(false);
        auto_mode_display->setVisible(true);
    }
    else
    {
        normal_modes_table->setVisible(false);
        start_mode_display->setVisible(false);
        stop_mode_display->setVisible(false);
        auto_mode_display->setVisible(false);
    }

    view_angles->setCurrentText(tr("Default"));

    play_widget_animations();
}

void MainWindow::editPrefs()
{
    if(nullptr == prefswindow)
    {
        prefswindow = new PrefsWindow(this);
        QObject::connect(prefswindow, &PrefsWindow::themePrefs_changed,
                         this, &MainWindow::icon_theme_changed);

        QObject::connect(prefswindow, &PrefsWindow::hideMenuBar_changed,
                         this, &MainWindow::menubar_hide);

        QObject::connect(prefswindow, &PrefsWindow::viewPortColour_changed,
                         this, &MainWindow::viewport_colour_changed);

        QObject::connect(prefswindow, &PrefsWindow::iconColour_changed, [this](bool automatic, bool dark)
        {
            if (automatic) icon_theme_changed();
            else icon_theme_user_changed(dark);
        });

    }

    prefswindow->setVisible(prefswindow->isHidden());
}

void MainWindow::sendView()
{
    m_modifier->views(view_angles);
}

void MainWindow::menubar_hide(QCheckBox *check_menu_hide)
{
    menuBar()->setVisible(!check_menu_hide->isChecked());
}

void MainWindow::playAnimation()
{
    start_xyz_display->setEnabled(false);
    stop_xyz_display->setEnabled(true);
    start_frame->setEnabled(true);
    end_frame->setEnabled(true);

//    m_modifier->play_xyz_animation(frameCount, stop_xyz_display);
    if(!play_xyz_timer->isActive() && play_xyz_timer)
    {
        frameCount->blockSignals(true);
        play_xyz_timer->start(750);
    }
}

void MainWindow::stopAnimation()
{
    end_frame->setEnabled(true);
    start_frame->setEnabled(true);
    start_xyz_display->setEnabled(true);
    stop_xyz_display->setEnabled(false);
    start_frame->setEnabled(true);

    if (frameCount->value() == frameCount->maximum()) end_frame->setEnabled(false);
    if (frameCount->value() == frameCount->minimum()) start_frame->setEnabled(false);

    if (play_xyz_timer)
        play_xyz_timer->stop();

    frameCount->blockSignals(false);
}


void MainWindow::setup_molview_signals_and_slots()
{
    QObject::connect(x_rotate, &QPushButton::pressed,
                     this, &MainWindow::sendX);

    QObject::connect(y_rotate, &QPushButton::pressed,
                     this, &MainWindow::sendY);

    QObject::connect(z_rotate, &QPushButton::pressed,
                     this, &MainWindow::sendZ);

    QObject::connect(view_angles, qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &MainWindow::sendView);

//    QObject::connect(r_reset, &QPushButton::clicked,
//                     m_modifier, &MolView::reset);


    QObject::connect(r_reset, &QPushButton::clicked,
                     this, &MainWindow::sendResetView);

    QObject::connect(frameCount, qOverload<int>(&QSpinBox::valueChanged),
                     this, &MainWindow::sendFrameIndex);

    QObject::connect(start_frame, &QPushButton::clicked,
                     this, &MainWindow::sendsetFirstFrame);

    QObject::connect(end_frame, &QPushButton::clicked,
                     this, &MainWindow::sendsetLastFrame);

    QObject::connect(m_modifier, &MolView::num_frames_changed, this->numFrames,
                     &QLabel::setText);

    QObject::connect(m_modifier, &MolView::frequencies_changed, this,
                     &MainWindow::populate_normal_modes_table);

    QObject::connect(start_mode_display, &QPushButton::clicked, this,
                     &MainWindow::sendplayNormalMode);

    QObject::connect(stop_mode_display, &QPushButton::clicked, this,
                     &MainWindow::sendstopNormalMode);

    QObject::connect(normal_modes_table, &QTableWidget::cellClicked, this,
                     &MainWindow::activatePlay);

    QObject::connect(start_xyz_display, &QPushButton::clicked, this,
                     &MainWindow::playAnimation);

    QObject::connect(stop_xyz_display, &QPushButton::clicked, this,
                     &MainWindow::stopAnimation);

    QObject::connect(set_atom_sizes, &QDial::valueChanged,
                     this, &MainWindow::sendAtomSizes);

    QObject::connect(atom_size_default, &QPushButton::clicked,
                     [this](){ set_atom_sizes->setValue(35); });

    play_mode_timer = new QTimer(this);
    QObject::connect(play_mode_timer, &QTimer::timeout, [this]()
    { m_modifier->play_normal_mode(normal_modes_table); });

    play_xyz_timer = new QTimer(this);
    QObject::connect(play_xyz_timer, &QTimer::timeout, [this]()
    { m_modifier->play_xyz_animation(frameCount, stop_xyz_display); });
}

void MainWindow::icon_theme_changed()
{
    end_frame->setIcon(set_icon_name(":/icons/media-seek-forward-symbolic"));
    start_frame->setIcon(set_icon_name(":/icons/media-seek-backward-symbolic"));
    start_frame->setIcon(set_icon_name(":/icons/media-seek-backward-symbolic"));
    start_mode_display->setIcon(set_icon_name(":/icons/media-playback-start-symbolic"));
    stop_mode_display->setIcon(set_icon_name(":/icons/media-playback-stop-symbolic"));

    openAct->setIcon(set_icon_name(":/icons/document-open-symbolic"));
    exitAct->setIcon(set_icon_name(":/icons/application-exit-symbolic"));
    prefsAct->setIcon(set_icon_name(":/icons/preferences-system-symbolic"));
}

void MainWindow::icon_theme_user_changed(bool dark)
{
    {
        end_frame->setIcon(set_icon_name(":/icons/media-seek-forward-symbolic", dark));
        start_frame->setIcon(set_icon_name(":/icons/media-seek-backward-symbolic", dark));
        start_mode_display->setIcon(set_icon_name(":/icons/media-playback-start-symbolic", dark));
        stop_mode_display->setIcon(set_icon_name(":/icons/media-playback-stop-symbolic", dark));

        openAct->setIcon(set_icon_name(":/icons/document-open-symbolic", dark));
        exitAct->setIcon(set_icon_name(":/icons/application-exit-symbolic", dark));
        prefsAct->setIcon(set_icon_name(":/icons/preferences-system-symbolic", dark));
    }
}

void MainWindow::viewport_colour_changed(QColor colour)
{
   view->defaultFrameGraph()->setClearColor(QColor(colour));
}

void MainWindow::populate_normal_modes_table(const QVector<double>& frequencies,
                                             const QVector<double>& intensities)
{
    normal_modes_table->setRowCount(0);
    const bool do_intensities = (intensities.size() > 0)
                              ? true : false;

    int k = 0;
    QStringList headers;
    auto font = normal_modes_table->font();

#ifdef Q_OS_LINUX
    font.setFamily("Source Code Pro");
#else
    font.setFamily("Lucida Console");
    font.setPointSize(10);
#endif

    font.setStyleHint(QFont::TypeWriter);

    for (const auto &freq : frequencies)
    {
        normal_modes_table->insertRow(k);
        if (k < 9)
            headers << QString("   ") + QString::number(k + 1);
        else
            headers << QString("  ") + QString::number(k + 1);

        QTableWidgetItem *newItem = new QTableWidgetItem;
        newItem->setTextAlignment(Qt::AlignRight);
        newItem->setText(QString::number(freq, 'f', 4) + QString("    "));
        newItem->setFont(font);
        normal_modes_table->setItem(k, 0, newItem);

        if (do_intensities)
        {
            QTableWidgetItem *newItem2 = new QTableWidgetItem;
            newItem2->setTextAlignment(Qt::AlignRight);
            newItem2->setText(QString::number(intensities[k], 'f', 4) + QString("    "));
            newItem2->setFont(font);
            normal_modes_table->setItem(k, 1, newItem2);
        }

        ++k;
    }

    normal_modes_table->setVerticalHeaderLabels(headers);
    normal_modes_table->setAutoScroll(true);
}

void MainWindow::about()
{
#ifdef Q_OS_LINUX
    auto desktop = qgetenv("XDG_CURRENT_DESKTOP");
#endif

    QMessageBox *aboutbox = new QMessageBox(this);
    aboutbox->setTextFormat(Qt::RichText);
    QSysInfo sysinfo = QSysInfo();
    aboutbox->setWindowTitle(tr("About MolView"));

    QString text(tr("<p> An application for viewing xyz Geometry files "
                     "and Normal Mode Molden files."
                 "<br><br><i><b><u>Author</u></b></i><br><br><a href=\"http://github.com/AlexB67\">Alexander Borro</a> "));

    text.append(tr("<br><br><i><b><u>Product Information</u></b></i><br><br>"));
    text.append(tr("<i><b> Version</b></i> : 0.1.0")); // To do get it from cmake

    text.append(tr("<br><br><i><b><u>System Information</u></b></i><br><br>"));

    text.append(tr("<i><b> OS Info</b></i> : ") + sysinfo.prettyProductName().section('(',0,0) + tr("(")
                + sysinfo.buildCpuArchitecture() + tr(")"));

#ifdef Q_OS_LINUX
    if (!desktop.isNull())
        text.append(tr("<br><i><b> Desktop</b></i> : " + desktop));
    else
        text.append(tr("<br><i> Desktop</i> : unknown"));
#endif

    text.append(tr("<br><i><b>Kernel</b></i> : ") + sysinfo.kernelVersion());

    text.append(tr("<br><br><i><b><u>Licence terms</u></b></i> "
                   "<br><br><a href=\"https://www.gnu.org/licenses/lgpl-3.0.en.html\">LGPL v3</a>"));

    aboutbox->setText(text);
    auto pixmap = QPixmap(":/icons/qmolview.png");
    aboutbox->setIconPixmap(pixmap.scaledToHeight(64));
    aboutbox->exec();

    delete aboutbox;
    return;
}
