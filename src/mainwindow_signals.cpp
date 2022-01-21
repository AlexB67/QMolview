#include "appglobals.h"
#include "mainwindow.h"
#include <QDesktopServices>
//#include <Qt3DRender/QRenderCaptureReply>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMimeDatabase>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTableWidgetItem>
#include <QSysInfo>
#include <QFontDatabase>

void MainWindow::sendX()
{
    if (!x_rotate->isCheckable())
        m_modifier->RotateX(x_rotate, view_angles, rotation_dir, snap_to_centre->isChecked());
    else
    {
        if (!x_rotate->isChecked())
        {
            if (x_rotate_timer)
            {
                if (snap_to_centre) m_modifier->resetViewCentre();
                x_rotate_timer->setInterval(10);
                x_rotate_timer->start();
            }
        }
        else
        {
            if (x_rotate_timer)
                x_rotate_timer->stop();
        }
    }
}

void MainWindow::sendY()
{
    if (!y_rotate->isCheckable())
        m_modifier->RotateY(y_rotate, view_angles, rotation_dir, snap_to_centre->isChecked());
    else
    {
        if (!y_rotate->isChecked())
        {
            if (y_rotate_timer)
            {
                if (snap_to_centre) m_modifier->resetViewCentre();
                y_rotate_timer->setInterval(10);
                y_rotate_timer->start();
            }
        }
        else
        {
            if (y_rotate_timer)
                y_rotate_timer->stop();
        }
    }
}

void MainWindow::sendZ()
{
    if (!z_rotate->isCheckable())
        m_modifier->RotateZ(z_rotate, view_angles, rotation_dir, snap_to_centre->isChecked());
    else
    {
        if (!z_rotate->isChecked())
        {
            if (z_rotate_timer)
            {
                if (snap_to_centre) m_modifier->resetViewCentre();
                z_rotate_timer->setInterval(10);
                z_rotate_timer->start();
            }
        }
        else
        {
            if (z_rotate_timer)
                z_rotate_timer->stop();
        }
    }
}

void MainWindow::setRotationMode()
{
    if (!rotationModeAct->isChecked())
    {
        x_rotate->setChecked(false);
        y_rotate->setChecked(false);
        z_rotate->setChecked(false);

        x_rotate->setCheckable(false);
        y_rotate->setCheckable(false);
        z_rotate->setCheckable(false);

        if (x_rotate_timer) x_rotate_timer->stop();
        if (y_rotate_timer) y_rotate_timer->stop();
        if (z_rotate_timer) z_rotate_timer->stop();
    }
    else
    {
        x_rotate->setCheckable(true);
        y_rotate->setCheckable(true);
        z_rotate->setCheckable(true);
    }
}

void MainWindow::setXrotateTimer()
{
    m_modifier->RotateX(rotation_dir);
}

void MainWindow::setYrotateTimer()
{
    m_modifier->RotateY(rotation_dir);
}

void MainWindow::setZrotateTimer()
{
    m_modifier->RotateZ(rotation_dir);
}

void MainWindow::sendAtomSizes()
{
    m_modifier->scale_atoms(set_atom_sizes);
    if (!atomsScaleAct->isEnabled()) atomsScaleAct->setEnabled(true);
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

void MainWindow::sendView()
{
    placeholderComboText->setVisible(view_angles->currentIndex() == -1);
    m_modifier->views(view_angles);
}

void MainWindow::sendplayNormalMode()
{   // start normal playing start button
    start_mode_display->setEnabled(false);
    stop_mode_display->setEnabled(true);
    pause_mode_display->setEnabled(true);

    if (mode_display_paused)
    {   // only call stop when paused
        mode_display_paused = false;
        if (play_mode_timer) play_mode_timer->stop();
    }

    if (play_mode_timer)
    {   // approx 25 msec default 127 on our scale
        // (Also, work around getting the same tick count on both amplitude and speed slide,
        // for appearance sake, want max 110, slider max 130)
        auto val = (animation_speed->value() > 110) ? 110 : animation_speed->value();
        int inter_val =
        static_cast<int>(std::pow(2.0, 8 - static_cast<double>(val) / 20.0));

        if(!play_mode_timer->isActive())
            play_mode_timer->start(inter_val);

        saveAsAct->setEnabled(false);
    }
}

void MainWindow::sendpauseNormalMode()
{   // Stop normal playing stop button
    start_mode_display->setEnabled(true);
    pause_mode_display->setEnabled(false);
    stop_mode_display->setEnabled(false);

    if (play_mode_timer)
        play_mode_timer->stop();

    saveAsAct->setEnabled(true);
    mode_display_paused = true;
}

void MainWindow::sendstopNormalMode()
{   // Stop normal playing stop button
    start_mode_display->setEnabled(true);
    pause_mode_display->setEnabled(false);
    stop_mode_display->setEnabled(false);

    if (play_mode_timer)
        play_mode_timer->stop();

    m_modifier->stop_normal_mode();
    saveAsAct->setEnabled(true);
}

void MainWindow::setNormalModeAnimationSpeed()
{
    if (play_mode_timer)
    {   // approx 25 msec default
        auto val = (animation_speed->value() > 110) ? 110 : animation_speed->value();
        int inter_val =
        static_cast<int>(std::pow(2.0, 8 - static_cast<double>(val) / 20.0));
        play_mode_timer->setInterval(inter_val);
        if (!defaultAnimAct->isEnabled()) defaultAnimAct->setEnabled(true);

        animinfo_label->setText(tr("Anim: ") + QString::number(inter_val) +
        + " msec | " + QString::number(static_cast<double>(amplitude->value()) / 100.0, 'f', 2));
    }
}

void MainWindow::activatePlay()
{   // Stop normal playing when clicking on a table row
    start_mode_display->setEnabled(true);
    stop_mode_display->setEnabled(false);

    if (play_mode_timer)
        play_mode_timer->stop();

    m_modifier->stop_normal_mode(); // resets coordinates

    if (auto_mode_display->isChecked())
        sendplayNormalMode();

    if (show_norm_vectors->checkState() == Qt::Checked || show_norm_vectors->checkState() == Qt::PartiallyChecked)
        m_modifier->set_norm_vectors(normal_modes_table, show_norm_vectors, static_cast<float>(amplitude->value()) / 100.0f);
}

void MainWindow::openFileDialog()
{

    if (play_mode_timer) play_mode_timer->stop();
    if (play_xyz_timer) play_xyz_timer->stop();
    if (x_rotate_timer)
    {
        x_rotate->setChecked(false);
        x_rotate_timer->stop();
    }

    if (y_rotate_timer)
    {
        y_rotate->setChecked(false);
        y_rotate_timer->stop();
    }

    if (z_rotate_timer)
    {
        z_rotate->setChecked(false);
        z_rotate_timer->stop();
    }

    QFileDialog::Options options;
    if (!SystemSettings::native_dialogs)
    options = QFlag(QFileDialog::DontUseNativeDialog);
    // remember the path for future visits
    static QString currentPath = QDir::homePath();

    QString selectedFilter;
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setModal(true);
    dialog->setNameFilter(tr("*.xyz *.cml *.molden_normal_modes"));
    dialog->setOption(QFileDialog::ReadOnly);
    dialog->setViewMode(QFileDialog::ViewMode::List);
    QString file = dialog->getOpenFileName(this, tr("Open a molecule xyz, cml, or normal mode Molden file"),
                                          currentPath, tr("*.xyz *.cml *.molden_normal_modes"),
                                          &selectedFilter, options);
    currentPath = file;
#ifdef Q_OS_WIN
    currentPath.truncate(currentPath.lastIndexOf(QChar('\\')));
#else
    currentPath.truncate(currentPath.lastIndexOf(QChar('/')));
#endif
    delete dialog;

    QFileInfo filext(file);
    if (!file.length())
        return;
    else if (filext.suffix() != tr("xyz") && filext.suffix() != tr("cml") && filext.suffix() != tr("molden_normal_modes"))
        return;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(file);
    mimeType->setText(QString(tr("Mime type: ")) + type.name());
#endif

    m_modifier->openFile(file, filext.suffix());
    ctrlBox->setEnabled(true);
    frameCount->setRange(1, static_cast<int>(m_modifier->get_frames_count()));
    frameCount->setValue(1);
    caminfo_label->setVisible(true);

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
        ctrlBox->updateGeometry();
    }

    if (filext.suffix() == "molden_normal_modes")
    {
        normal_modes_table->setVisible(true);
        start_mode_display->setVisible(true);
        start_mode_display->setEnabled(false);
        pause_mode_display->setVisible(true);
        pause_mode_display->setEnabled(false);
        stop_mode_display->setVisible(true);
        stop_mode_display->setEnabled(false);
        auto_mode_display->setEnabled(true);
        animation_speed_label->setVisible(true);
        amplitude_label->setVisible(true);
        animation_speed->setVisible(true);
        amplitude->setVisible(true);
        show_norm_vectors->setEnabled(true);
        animation_speed->setValue(67);
        Q_EMIT amplitude->valueChanged(75.0);
    }
    else
    {
        normal_modes_table->setVisible(false);
        start_mode_display->setVisible(false);
        pause_mode_display->setVisible(false);
        stop_mode_display->setVisible(false);
        auto_mode_display->setEnabled(false);
        animation_speed_label->setVisible(false);
        amplitude_label->setVisible(false);
        animation_speed->setVisible(false);
        amplitude->setVisible(false);
        show_norm_vectors->setEnabled(false);
        animinfo_label->setText("");
        viewSpectrumAct->setEnabled(false);
        spectrumwindow->hide();
    }

    sendResetView();
    set_atom_sizes->setValue(35);

    show_axes->blockSignals(true);
    show_axes->setChecked(false);
    show_axes->blockSignals(false);

    show_norm_vectors->blockSignals(true);
    show_norm_vectors->setChecked(false);
    show_norm_vectors->blockSignals(false);

    viewBox->setEnabled(true);
    toolbarctrl->setEnabled(true);
    defaultAnimAct->setEnabled(false);
    atomsScaleAct->setEnabled(false);
    saveAsAct->setEnabled(true);
    play_widget_animations();
    m_modifier->setFocus();
}

void MainWindow::saveAsxyz()
{
    if (play_mode_timer) play_mode_timer->stop();
    if (play_xyz_timer) play_xyz_timer->stop();
    if (x_rotate_timer)
    {
        x_rotate->setChecked(false);
        x_rotate_timer->stop();
    }

    if (y_rotate_timer)
    {
        y_rotate->setChecked(false);
        y_rotate_timer->stop();
    }

    if (z_rotate_timer)
    {
        z_rotate->setChecked(false);
        z_rotate_timer->stop();
    }

    QFileDialog::Options options;
    if (!SystemSettings::native_dialogs)
    options = QFlag(QFileDialog::DontUseNativeDialog);

    QString selectedFilter;
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setModal(true);
    dialog->setNameFilter(tr("*.xyz"));
    dialog->setViewMode(QFileDialog::ViewMode::List);

    QString file = dialog->getSaveFileName(this, tr("Save Normal Mode as an xyz file"),
                                             QDir::homePath(), tr("*.xyz"), &selectedFilter,  options);
    if (file.length())
        m_modifier->write_xyz(file);

    delete dialog;
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

        QObject::connect(prefswindow, &PrefsWindow::dialogStyle_changed,
                         this, &MainWindow::set_dialog_style);

        QObject::connect(prefswindow, &PrefsWindow::MenuBarStyle_changed,
                         this, &MainWindow::set_menubar_style);

        QObject::connect(prefswindow, &PrefsWindow::viewPortColour_changed,
                         this, &MainWindow::viewport_colour_changed);

        QObject::connect(prefswindow, &PrefsWindow::viewport_Intensity_changed,
                         this, &MainWindow::viewport_light_changed);

        QObject::connect(prefswindow, &PrefsWindow::iconColour_changed, this, [this](bool automatic, bool dark)
        {
            if (automatic) icon_theme_changed();
            else icon_theme_user_changed(dark);
        });

        QObject::connect(prefswindow, &PrefsWindow::mouse_speed_changed,
                         this, &MainWindow::viewport_mouse_speed_changed);

        QObject::connect(prefswindow, &PrefsWindow::iconSize_Changed,
                         this, &MainWindow::icon_size_changed);

        QObject::connect(prefswindow, &PrefsWindow::chart_Theme_Changed,
                         spectrumwindow, &SpectrumWindow::set_chart_theme);

        QObject::connect(prefswindow, &PrefsWindow::spectrum_axis_title_font_Changed,
                         spectrumwindow, &SpectrumWindow::set_plot_axis_title_font);

        QObject::connect(prefswindow, &PrefsWindow::spectrum_axis_label_font_Changed,
                         spectrumwindow, &SpectrumWindow::set_plot_axis_label_font);

        QObject::connect(prefswindow, &PrefsWindow::spectrum_titleVisible_Changed,
                         spectrumwindow, &SpectrumWindow::set_plot_title_visible);

        QObject::connect(prefswindow, &PrefsWindow::spectrum_line_Colour_Changed,
                         spectrumwindow, &SpectrumWindow::set_plot_line_colour);

        QObject::connect(prefswindow, &PrefsWindow::spectrum_auto_line_Colour_Changed,
                         spectrumwindow, &SpectrumWindow::set_plot_auto_line_colour);

        // file import/export
        QObject::connect(prefswindow, &PrefsWindow::connectivity_Index_Changed,
        this, [this](bool yesno) { m_modifier->setConnectivityFromLastFrame(yesno); });

        QObject::connect(prefswindow, &PrefsWindow::sort_Axes_Changed,
        this, [this](bool yesno) { m_modifier->setSortAxes(yesno); });

        QObject::connect(prefswindow, &PrefsWindow::align_Axes_Changed,
        this, [this](bool yesno) { m_modifier->setAlignAxes(yesno); });

    }

    prefswindow->setVisible(prefswindow->isHidden());

    if (prefswindow->isVisible())
    {    // Pre gnome 40 and KDE subwindow placement is terrible - so override
        prefswindow->raise();
        prefswindow->move(geometry().topLeft().x() + geometry().width() / 2
                          - prefswindow->geometry().width() / 2,
                          geometry().topLeft().y() + geometry().height() / 2
                          - prefswindow->geometry().height() / 2);
    }
}

void MainWindow::showShortcuts()
{
    if(nullptr == shortcutswindow)
        shortcutswindow = new ShortCutsWindow(this);

    shortcutswindow->setVisible(shortcutswindow->isHidden());

     if (shortcutswindow->isVisible()) shortcutswindow->raise();
}

void MainWindow::showSpectrum()
{
    if(nullptr == spectrumwindow)
    {
        spectrumwindow = new SpectrumWindow(this);

        QObject::connect(m_modifier, &MolView::frequencies_changed, spectrumwindow,
                         &SpectrumWindow::init_spectrum);
    }

    spectrumwindow->setVisible(spectrumwindow->isHidden());
    if (spectrumwindow->isVisible()) spectrumwindow->raise();

    spectrumwindow->resize(QSize(width(), height()));
    spectrumwindow->move(this->pos().x() + 50, this->pos().y() + 50);
    spectrumwindow->play_animations();
}

void MainWindow::playAnimation()
{
    start_xyz_display->setEnabled(false);
    stop_xyz_display->setEnabled(true);
    start_frame->setEnabled(true);
    end_frame->setEnabled(true);

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

    QObject::connect(r_reset, &QPushButton::clicked,
                     this, &MainWindow::sendResetView);

    QObject::connect(frameCount, qOverload<int>(&QSpinBox::valueChanged),
                     this, &MainWindow::sendFrameIndex);

    QObject::connect(start_frame, &QPushButton::clicked,
                     this, &MainWindow::sendsetFirstFrame);

    QObject::connect(end_frame, &QPushButton::clicked,
                     this, &MainWindow::sendsetLastFrame);

    QObject::connect(m_modifier, &MolView::file_info_changed, fileinfo_label,
                     &QLabel::setText);

    QObject::connect(m_modifier, &MolView::frequencies_changed, this,
                     &MainWindow::populate_normal_modes_table);

    QObject::connect(start_mode_display, &QPushButton::clicked, this,
                     &MainWindow::sendplayNormalMode);

    QObject::connect(pause_mode_display, &QPushButton::clicked, this,
                     &MainWindow::sendpauseNormalMode);

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

    QObject::connect(animation_speed, &QSlider::valueChanged, this,
                     &MainWindow::setNormalModeAnimationSpeed);

    QObject::connect(amplitude, &QSlider::valueChanged,
                     this, &MainWindow::amplitudeChanged);

    QObject::connect(show_axes, &QCheckBox::stateChanged,
                     this, &MainWindow::showAxesChanged);

    QObject::connect(show_norm_vectors, &QCheckBox::stateChanged,
                     this, &MainWindow::setNormalModeVectors);

    // Timers
    // normal mode animation
    play_mode_timer = new QTimer(this);
    play_mode_timer->setTimerType(Qt::TimerType::PreciseTimer);

    QObject::connect(play_mode_timer, &QTimer::timeout,
                     this, &MainWindow::setModeTimer);
    // xyz animation
    play_xyz_timer = new QTimer(this);
    play_xyz_timer->setTimerType(Qt::TimerType::PreciseTimer);

    QObject::connect(play_xyz_timer, &QTimer::timeout,
                     this, &MainWindow::setXYZTimer);
    // rotation
    x_rotate_timer = new QTimer(this);
    x_rotate_timer->setTimerType(Qt::TimerType::PreciseTimer);

    QObject::connect(x_rotate_timer, &QTimer::timeout,
                     this, &MainWindow::setXrotateTimer);

    y_rotate_timer = new QTimer(this);
    y_rotate_timer->setTimerType(Qt::TimerType::PreciseTimer);

    QObject::connect(y_rotate_timer, &QTimer::timeout,
                     this, &MainWindow::setYrotateTimer);

    z_rotate_timer = new QTimer(this);
    z_rotate_timer->setTimerType(Qt::TimerType::PreciseTimer);

    QObject::connect(z_rotate_timer, &QTimer::timeout,
                     this, &MainWindow::setZrotateTimer);
}

void MainWindow::icon_theme_changed()
{
    end_frame->setIcon(set_icon_name(":/icons/media-seek-forward-symbolic"));
    start_frame->setIcon(set_icon_name(":/icons/media-seek-backward-symbolic"));
    start_mode_display->setIcon(set_icon_name(":/icons/media-playback-start-symbolic"));
    pause_mode_display->setIcon(set_icon_name(":/icons/media-playback-pause-symbolic"));
    stop_mode_display->setIcon(set_icon_name(":/icons/media-playback-stop-symbolic"));
    start_xyz_display->setIcon(set_icon_name(":/icons/media-playback-start-symbolic"));
    stop_xyz_display->setIcon(set_icon_name(":/icons/media-playback-stop-symbolic"));
    auto_mode_display->setIcon(set_icon_name(":/icons/view-continuous-symbolic"));
    x_rotate->setButtonIcon(set_icon_name(":/icons/view-refresh-symbolic"));
    y_rotate->setButtonIcon(set_icon_name(":/icons/view-refresh-symbolic"));
    z_rotate->setButtonIcon(set_icon_name(":/icons/view-refresh-symbolic"));
    show_axes->setIcon(set_icon_name(":/icons/axis-symbolic"));
    show_norm_vectors->setIcon(set_icon_name(":/icons/vector-symbolic"));

    openAct->setIcon(set_icon_name(":/icons/document-open-symbolic"));
    saveAsAct->setIcon(set_icon_name(":/icons/document-save-as-symbolic"));
    aboutAct->setIcon(set_icon_name(":/icons/about-symbolic"));
    aboutQtAct->setIcon(set_icon_name(":/icons/about-symbolic"));
    helpAct->setIcon(set_icon_name(":/icons/help-browser-symbolic"));
    exitAct->setIcon(set_icon_name(":/icons/application-exit-symbolic"));
    prefsAct->setIcon(set_icon_name(":/icons/preferences-system-symbolic"));
    shortcutsAct->setIcon(set_icon_name(":/icons/keyboard-shortcuts-symbolic"));
    viewSpectrumAct->setIcon(set_icon_name(":/icons/video-display-symbolic"));
    rotateleftAct->setIcon(set_icon_name(":/icons/object-rotate-left-symbolic"));
    rotaterightAct->setIcon(set_icon_name(":/icons/object-rotate-right-symbolic"));
    rotationModeAct->setIcon(set_icon_name(":/icons/view-refresh-symbolic"));
    atomsScaleAct->setIcon(set_icon_name(":/icons/view-restore-symbolic"));
    defaultAnimAct->setIcon(set_icon_name(":/icons/emblem-default-symbolic"));
    toolButton->setIcon(set_icon_name(":/icons/open-menu-symbolic"));

#ifdef Q_OS_LINUX
    hamBurgerMenu->setStyleSheet(get_menu_style_sheet());
    hamBurgerMenu->update();
#endif
}

void MainWindow::icon_theme_user_changed(bool dark)
{
    end_frame->setIcon(set_icon_name(":/icons/media-seek-forward-symbolic", dark));
    start_frame->setIcon(set_icon_name(":/icons/media-seek-backward-symbolic", dark));
    start_mode_display->setIcon(set_icon_name(":/icons/media-playback-start-symbolic", dark));
    pause_mode_display->setIcon(set_icon_name(":/icons/media-playback-pause-symbolic", dark));
    stop_mode_display->setIcon(set_icon_name(":/icons/media-playback-stop-symbolic", dark));
    start_xyz_display->setIcon(set_icon_name(":/icons/media-playback-start-symbolic", dark));
    stop_xyz_display->setIcon(set_icon_name(":/icons/media-playback-stop-symbolic", dark));
    auto_mode_display->setIcon(set_icon_name(":/icons/view-continuous-symbolic", dark));
    x_rotate->setButtonIcon(set_icon_name(":/icons/view-refresh-symbolic", dark));
    y_rotate->setButtonIcon(set_icon_name(":/icons/view-refresh-symbolic", dark));
    z_rotate->setButtonIcon(set_icon_name(":/icons/view-refresh-symbolic", dark));
    show_axes->setIcon(set_icon_name(":/icons/axis-symbolic", dark));
    show_norm_vectors->setIcon(set_icon_name(":/icons/vector-symbolic", dark));

    openAct->setIcon(set_icon_name(":/icons/document-open-symbolic", dark));
    saveAsAct->setIcon(set_icon_name(":/icons/document-save-as-symbolic", dark));
    aboutAct->setIcon(set_icon_name(":/icons/about-symbolic", dark));
    aboutQtAct->setIcon(set_icon_name(":/icons/about-symbolic", dark));
    helpAct->setIcon(set_icon_name(":/icons/help-browser-symbolic", dark));
    exitAct->setIcon(set_icon_name(":/icons/application-exit-symbolic", dark));
    prefsAct->setIcon(set_icon_name(":/icons/preferences-system-symbolic", dark));
    shortcutsAct->setIcon(set_icon_name(":/icons/keyboard-shortcuts-symbolic", dark));
    viewSpectrumAct->setIcon(set_icon_name(":/icons/video-display-symbolic", dark));
    rotateleftAct->setIcon(set_icon_name(":/icons/object-rotate-left-symbolic", dark));
    rotaterightAct->setIcon(set_icon_name(":/icons/object-rotate-right-symbolic", dark));
    rotationModeAct->setIcon(set_icon_name(":/icons/view-refresh-symbolic", dark));
    atomsScaleAct->setIcon(set_icon_name(":/icons/view-restore-symbolic", dark));
    defaultAnimAct->setIcon(set_icon_name(":/icons/emblem-default-symbolic", dark));
    toolButton->setIcon(set_icon_name(":/icons/open-menu-symbolic", dark));

#ifdef Q_OS_LINUX
    hamBurgerMenu->setStyleSheet(get_menu_style_sheet());
    hamBurgerMenu->update();
#endif
}

void MainWindow::icon_size_changed(int icon_size)
{
    if (icon_size == 0)
    {
        toolbar->setIconSize(QSize(16, 16));
        toolbaredit->setIconSize(QSize(16, 16));
        toolbarhelp->setIconSize(QSize(16, 16));
        toolbarview->setIconSize(QSize(16, 16));
        toolbarctrl->setIconSize(QSize(16, 16));
    }
    else if (icon_size == 1)
    {
        toolbar->setIconSize(QSize(20, 20));
        toolbaredit->setIconSize(QSize(20, 20));
        toolbarhelp->setIconSize(QSize(20, 20));
        toolbarview->setIconSize(QSize(20, 20));
        toolbarctrl->setIconSize(QSize(20, 20));
    }
    else
    {
        toolbar->setIconSize(QSize(24, 24));
        toolbaredit->setIconSize(QSize(24, 24));
        toolbarhelp->setIconSize(QSize(24, 24));
        toolbarview->setIconSize(QSize(24, 24));
        toolbarctrl->setIconSize(QSize(24, 24));
    }
}

void MainWindow::viewport_light_changed(int intensity)
{
    view->set_viewport_Intensity(intensity);
}

void MainWindow::menubar_hide(QCheckBox *check_menu_hide)
{
    menuBar()->setHidden(check_menu_hide->isChecked());
    menuBar()->setEnabled(!check_menu_hide->isChecked());
}

void MainWindow::set_menubar_style(int style)
{

    if (style == 0) // menu bar
    {
        menuBar()->setEnabled(true);
        menuBar()->setHidden(false);
        toolbarmenu->setVisible(false);
        toolbaredit->setVisible(true);
        toolbarhelp->setVisible(true);
    }
    else // gnome style
    {
        menuBar()->setEnabled(false);
        menuBar()->setHidden(true);
        toolbarmenu->setVisible(true);
        toolbaredit->setVisible(false);
        toolbarhelp->setVisible(false);
    }
}

void MainWindow::set_dialog_style(bool native)
{
    SystemSettings::native_dialogs = native;
}

void MainWindow::viewport_mouse_speed_changed(int speed)
{
    view->get_cam_controller()->set_camera_mouse_speed(speed);
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

    viewSpectrumAct->setEnabled(do_intensities);

    if (!do_intensities)
    {
        viewSpectrumAct->setEnabled(false);
        if (spectrumwindow) spectrumwindow->hide();
    }
    else
        viewSpectrumAct->setEnabled(true);

    int k = 0;
    QStringList headers;
    auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);

#ifdef Q_OS_WIN
    font.setFamily(tr("Lucida Console"));
    font.setPointSizeF(1.2 * font.pointSizeF());
#else
    font.setStyleHint(QFont::TypeWriter);
#endif


    for (const auto &freq : frequencies)
    {
        normal_modes_table->insertRow(k);
        if (k < 9)
            headers << QString("   ") + QString::number(k + 1);
        else
            headers << QString("  ") + QString::number(k + 1);

        QTableWidgetItem *newItem = new QTableWidgetItem;
        newItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        newItem->setText(QString::number(freq, 'f', 4) + QString("    "));
        newItem->setFont(font);
        normal_modes_table->setItem(k, 0, newItem);

        if (do_intensities)
        {
            QTableWidgetItem *newItem2 = new QTableWidgetItem;
            newItem2->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            newItem2->setText(QString::number(intensities[k], 'f', 4) + QString("    "));
            newItem2->setFont(font);
            normal_modes_table->setItem(k, 1, newItem2);
        }

        ++k;
    }

    normal_modes_table->setVerticalHeaderLabels(headers);
    normal_modes_table->setAutoScroll(true);
}

void MainWindow::rotateLeft()
{
    x_rotate->setButtonText(tr("X-"));
    y_rotate->setButtonText(tr("Y-"));
    z_rotate->setButtonText(tr("Z-"));

    x_rotate->mirrorIcon(true, false);
    y_rotate->mirrorIcon(true, false);
    z_rotate->mirrorIcon(true, false);

    rotation_dir = -1.0f;
    rotateleftAct->setEnabled(false);
    rotaterightAct->setEnabled(true);
}

void MainWindow::rotateRight()
{
    x_rotate->setButtonText(tr("X+"));
    y_rotate->setButtonText(tr("Y+"));
    z_rotate->setButtonText(tr("Z+"));

    x_rotate->mirrorIcon(false, false);
    y_rotate->mirrorIcon(false, false);
    z_rotate->mirrorIcon(false, false);

    rotation_dir = 1.0f;
    rotateleftAct->setEnabled(true);
    rotaterightAct->setEnabled(false);
}

void MainWindow::setDefaultAtomScale()
{
    set_atom_sizes->setValue(35);
    atomsScaleAct->setEnabled(false);
}

void MainWindow::setDefaultAnimationParams()
{
    animation_speed->setValue(67);
    amplitude->setValue(75);
    defaultAnimAct->setEnabled(false);
}

void MainWindow::showAxesChanged()
{
    if (show_axes->checkState() == Qt::PartiallyChecked)
         m_modifier->reveal_axes(true, false);
    else if (show_axes->checkState() == Qt::Checked)
        m_modifier->reveal_axes(true, true);
    else
         m_modifier->reveal_axes(false, false);
}

void MainWindow::amplitudeChanged()
{
    defaultAnimAct->setEnabled(true);
    int inter_val =
    static_cast<int>(std::pow(2.0, 8 - static_cast<double>(animation_speed->value()) / 20.0));
    animinfo_label->setText(tr("Anim: ") + QString::number(inter_val) +
    + " msec | " + QString::number(static_cast<double>(amplitude->value()) / 100.0, 'f', 2));

    m_modifier->set_norm_vectors(normal_modes_table, show_norm_vectors,
                                 static_cast<float>(amplitude->value()) / 100.0f);
}

void MainWindow::setXYZTimer()
{
    m_modifier->play_xyz_animation(frameCount, stop_xyz_display);
}

void MainWindow::setModeTimer()
{
    m_modifier->play_normal_mode(normal_modes_table,
                static_cast<float>(amplitude->value()) / 100.0f);
}

void MainWindow::setNormalModeVectors()
{
    m_modifier->set_norm_vectors(normal_modes_table, show_norm_vectors, static_cast<float>(amplitude->value()) / 100.0f);
}

void MainWindow::about()
{
#ifdef Q_OS_LINUX
    auto desktop = qgetenv("XDG_CURRENT_DESKTOP");
#endif

    QMessageBox *aboutbox = new QMessageBox(this);
    aboutbox->setTextFormat(Qt::RichText);
    QSysInfo sysinfo = QSysInfo();
    aboutbox->setWindowTitle(tr("About QMolView"));

    auto text(tr("<p> An application for viewing xyz geometry, cml"
                     "<br>and normal mode Molden files."
                 "<br><br><i><b><u>Author</u></b></i><br><br><a href=\"http://github.com/AlexB67\">Alexander Borro</a> "));

    text.append(tr("<br><br><i><b><u>Product Information</u></b></i><br><br>"));
    text.append(tr("<i><b> Version</b></i> : "));
    text.append(QCoreApplication::applicationVersion());
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
    aboutbox->setContentsMargins(20, 20, 20, 20);
    aboutbox->exec();

    delete aboutbox;
    return;
}

void MainWindow::launchHelp()
{
#ifdef Q_OS_LINUX
    const auto doc_location = SystemSettings::install_preFix + QString("/share/qmolview/doc/qmolviewmanual.pdf");
#endif

#ifdef Q_OS_WIN
    const auto doc_location = QString("doc/qmolviewmanual.pdf");
#endif

    QDesktopServices::openUrl(QUrl(doc_location));
}
