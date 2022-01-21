#include "appglobals.h"
#include "spectrum_window.h"
#include <QPropertyAnimation>
#include <QSettings>
#include <iostream>

SpectrumWindow::SpectrumWindow(QWidget *parent) 
: QWidget(parent)
{
    setWindowFlags(Qt::Window);
    setWindowTitle(tr("Spectrum Viewer"));

    grid = new QGridLayout(this);
    grid->setSpacing(10);
    grid->setContentsMargins(10, 10, 10, 10);

    Xtracker = new QLineEdit(this);
    Xtracker->setReadOnly(true);
    Xtracker->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    Xtracker->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    Ytracker = new QLineEdit(this);
    Ytracker->setReadOnly(true);
    Ytracker->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    Ytracker->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    Ytracker->setContentsMargins(0, 0, 5, 0);

    Xtracker_label = new QLabel(this);
    Xtracker_label->setTextFormat(Qt::MarkdownText);
    Xtracker_label->setText(tr("𝜈 / cm<sup> -1</sup>"));
    Xtracker_label->setContentsMargins(10, 0, 10, 0);
    Xtracker_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    Ytracker_label = new QLabel(this);
    Ytracker_label->setTextFormat(Qt::MarkdownText);
    Ytracker_label->setText(tr("<i>ε</i> / L mol <sup> -1</sup> cm<sup> -1</sup>"));
    Ytracker_label->setContentsMargins(10, 0, 10, 0);
    Ytracker_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    spectrum = new QChart();
    spectrum->setTitle(tr("IR Spectrum"));
    spectrum_view = new SpectrumView(Xtracker, Ytracker, spectrum, this);

    FWHH_label = new QLabel(this);
    FWHH_label->setTextFormat(Qt::MarkdownText);
    FWHH_label->setText(tr("<i>w</i> / cm<sup> -1</sup>"));
    FWHH_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    freq_scaling_label = new QLabel(this);
    freq_scaling_label->setTextFormat(Qt::MarkdownText);
    freq_scaling_label->setText(tr("𝜈 - scaling"));
    freq_scaling_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    freq_scaling_type_label = new QLabel(this);
    freq_scaling_type_label->setTextFormat(Qt::MarkdownText);
    freq_scaling_type_label->setText(tr("𝜈 - scaling type"));
    freq_scaling_type_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    FWHH_spinner = new QDoubleSpinBox(this);
    FWHH_spinner->setToolTip(tr("Full Width at Half Height in wavenumbers."));
    FWHH_spinner->setValue(12.0);
    FWHH_spinner->setRange(1.0, 100.0);
    FWHH_spinner->setSingleStep(0.5);
    FWHH_spinner->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    flip_Yaxis = new QCheckBox(tr("Flip Y axis"), this);
    flip_Yaxis->setChecked(false);

    flip_Xaxis = new QCheckBox(tr("Flip X axis"), this);
    flip_Xaxis->setChecked(true);

    hide_selection = new QCheckBox(tr("Hide selection"), this);
    hide_selection->setToolTip(tr("Hide the peak selection line."));
    hide_selection->setChecked(false);

    defaults = new QPushButton(tr("Defaults"), this);
    defaults->setToolTip(tr("Reset the spectrum with default values."));

    freq_scaling = new QDoubleSpinBox(this);
    freq_scaling->setToolTip(tr("Scale Frequencies by the specified percentage."));
    freq_scaling->setRange(80.0, 100);
    freq_scaling->setMaximum(100.0);
    freq_scaling->setValue(100.0);
    freq_scaling->setSingleStep(0.1);
    freq_scaling->setDecimals(1);
    freq_scaling->setSuffix("%");
    freq_scaling->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    freq_scaling_type = new QComboBox(this);
    freq_scaling_type->setToolTip(tr("Frequency scaling algorithm.\n"
                                     "Linear: Scales frequencies by the same percentage .\n"
                                     "Relative: Scales frequencies in proportion to their wavenumber and percentage."));
    freq_scaling_type->addItem(tr("Linear"));
    freq_scaling_type->addItem(tr("Relative"));
    freq_scaling_type->setCurrentIndex(0);
    freq_scaling_type->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    line_shape_label = new QLabel(tr("Line shape"));

    line_shape_slider = new Slider(this);
    line_shape_slider->setToolTip(tr("The amount of Lorentzian versus Gaussian line shape character.\n"
                                     "The default left setting is 100% Lorentzian. The far right 100% Gaussian."));
    line_shape_slider->setRange(0, 100);
    line_shape_slider->setTickPosition(QSlider::TicksAbove);
    line_shape_slider->setValue(0);
    line_shape_slider->setSingleStep(1.0);
    line_shape_slider->setPageStep(10);
    line_shape_slider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    create_freq_table();

    spectrum_box = new QFrame(this);
    //spectrum_box->setAlignment(Qt::AlignCenter);

    spectrum_grid = new QGridLayout(spectrum_box);
    spectrum_grid->setContentsMargins(10, 10, 10, 10);
    spectrum_grid->setSpacing(10);

    control_box = new QGroupBox(tr("Manipulate"), this);
    control_box->setAlignment(Qt::AlignCenter);

    control_grid = new QGridLayout(control_box);
    control_grid->setContentsMargins(10, 10, 10, 10);
    control_grid->setSpacing(20);

    // spectrum area
    spectrum_grid->addWidget(spectrum_view, 0, 0, 1, 8);
    spectrum_grid->addWidget(flip_Xaxis, 1, 0, Qt::AlignRight);
    spectrum_grid->addWidget(flip_Yaxis, 1, 1, Qt::AlignRight);
    spectrum_grid->addWidget(Xtracker_label, 1, 4, Qt::AlignRight);
    spectrum_grid->addWidget(Xtracker, 1, 5);
    spectrum_grid->addWidget(Ytracker_label, 1, 6, Qt::AlignRight);
    spectrum_grid->addWidget(Ytracker, 1, 7);

    // control area
    control_grid->addWidget(FWHH_label, 0, 0);
    control_grid->addWidget(FWHH_spinner, 0, 1);
    control_grid->addWidget(freq_scaling_label, 0, 2);
    control_grid->addWidget(freq_scaling, 0, 3);
    control_grid->addWidget(freq_scaling_type_label, 1, 0);
    control_grid->addWidget(freq_scaling_type, 1, 1);
    control_grid->addWidget(line_shape_label, 1, 2);
    control_grid->addWidget(line_shape_slider, 1, 3);
    control_grid->addWidget(defaults, 2, 0,  Qt::AlignLeft);
    control_grid->addWidget(hide_selection, 2, 3, Qt::AlignLeft);
    control_grid->addWidget(freq_table, 0, 4, 3, 3);

    grid->addWidget(spectrum_box, 0, 0);
    grid->addWidget(control_box, 1, 0);

    setTabOrder(FWHH_spinner, freq_scaling);
    setTabOrder(freq_scaling, freq_scaling_type);

    setLayout(grid);

    QObject::connect(freq_scaling,  QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &SpectrumWindow::update_plot_with_freq_scaling);

    QObject::connect(freq_scaling_type,  qOverload<int>(&QComboBox::currentIndexChanged),
                     this, &SpectrumWindow::update_plot_with_freq_scaling);

    QObject::connect(FWHH_spinner,  QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &SpectrumWindow::update_plot);

    QObject::connect(line_shape_slider, &Slider::valueChanged, this, &SpectrumWindow::update_plot);
    QObject::connect(flip_Yaxis, &QCheckBox::clicked, this, &SpectrumWindow::flip_Y_axis);
    QObject::connect(flip_Xaxis, &QCheckBox::clicked, this, &SpectrumWindow::flip_X_axis);
    QObject::connect(freq_table, &QTableWidget::clicked, this, &SpectrumWindow::show_assignment_line);
    QObject::connect(hide_selection, &QCheckBox::clicked, this, &SpectrumWindow::show_assignment_line);

    QObject::connect(defaults,  &QPushButton::clicked,
                     [this](){ set_spectrum_defaults(); update_plot_with_freq_scaling();});
}

void SpectrumWindow::new_plot_points()
{   // First time population of plot points only
    QLineSeries& line_series = spectrum_view->get_line_series();

    const double fwhh = FWHH_spinner->value();
    const double fwhh2 = fwhh * fwhh;
    // 27.648045421; Naperian convention conversion factor
    const double prefac = fwhh * 2.0 * 100.0 / (M_PI * std::log(10.0));
    const double gprefac = 4.0 * std::log(2.0);
    const double fraction = static_cast<double>(line_shape_slider->value()) / 100.0;

    spectrum->blockSignals(true);
    line_series.blockSignals(true);

    xmin = std::floor(frequencies[0] / 100) * 100 - 100;
    if (xmin < 0) xmin = 0;
    xmax = std::ceil(frequencies[frequencies.size() - 1] / 100.0) * 100.0 + 200.0;

    double x = xmin;
    ymax = 0;
    while (x < xmax)
    {
        double eps = 0;
        bool near_peak = false;
        for (int i = 0; i < frequencies.size(); ++i)
        {
            if (intensities[i] > 1E-2)
            {
                // eps += prefac * intensities[i]
                // / (4.0 * (x - scale_vec[i] * frequencies[i]) * (x - scale_vec[i] * frequencies[i]) + fwhh * fwhh);
                const double delta = x - scale_vec[i] * frequencies[i];
                const double L = prefac * intensities[i] / (4.0 * delta * delta + fwhh2);
                const double G = prefac * intensities[i] *
                std::exp( - gprefac * delta * delta / fwhh2) / (fwhh * fwhh); // Normalize height
                eps += fraction * G + (1.0 - fraction) * L;
                if (std::fabs(delta) < 2.0 * gprefac * fwhh) near_peak = true; // Rather conservative. Don't miss a peak
            }
        }

        line_series.append(x, eps);

        if (eps > ymax) ymax = eps;

        if (!near_peak) x += 2.5;
        else x += 0.1 * fwhh; // Get roughly 10 points on peak shape
    };

    line_series.append(xmax, 0.0); // make sure we reach the maximum

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    points_vec = line_series.pointsVector();
#else
    points_vec = line_series.points();
#endif

    spectrum->blockSignals(false);
    line_series.blockSignals(false);
}

void SpectrumWindow::replace_plot_points()
{   // plot point updating
    // we allow the vector to grow if needed
    // The number of points is not constant
    QLineSeries& line_series = spectrum_view->get_line_series();

    const double fwhh = FWHH_spinner->value();
    const double fwhh2 = fwhh * fwhh;
    // 27.648045421; Naperian convention conversion factor
    const double prefac = fwhh * 2.0 * 100.0 / (M_PI * std::log(10.0));
    const double gprefac = 4.0 * std::log(2.0);
    const double fraction = static_cast<double>(line_shape_slider->value()) / 100.0;

    double x = xmin;
    const auto current_size = points_vec.size();

    xmin = std::floor(scale_vec[0] * frequencies[0] / 100) * 100 - 100;
    if (xmin < 0) xmin = 0;

    xmax = std::ceil(scale_vec[scale_vec.size() - 1] * frequencies[frequencies.size() - 1] / 100.0)
         * 100.0 + 200.0;

    ymax = 0;
    int idx = 0;
    while (x < xmax)
    {
        double eps = 0;
        bool near_peak = false;
        for (int i = 0; i < frequencies.size(); ++i)
        {
            if (intensities[i] > 1E-2)
            {
                // eps += prefac * intensities[i]
                // / (4.0 * (x - scale_vec[i] * frequencies[i]) * (x - scale_vec[i] * frequencies[i]) + fwhh * fwhh);
                const double delta = x - scale_vec[i] * frequencies[i];
                const double L = prefac * intensities[i] / (4.0 * delta * delta + fwhh2);
                const double G = prefac * intensities[i] *
                std::exp( - gprefac * delta * delta / fwhh2) / (fwhh * fwhh);
                eps += fraction * G + (1.0 - fraction) * L;
                if (std::fabs(delta) < 2.0 * gprefac * fwhh) near_peak = true; // Rather conservative. Don't miss a peak
            }
        }

        if (idx < current_size) points_vec[idx] = QPointF(x, eps);
        else points_vec.push_back(QPointF(x, eps));

        if (eps > ymax) ymax = eps;

        if (!near_peak) x += 2.5;
        else x += 0.1 * fwhh; // Get roughly 10 points on peak shape

        ++idx;
    };

    if (idx >= current_size) points_vec.push_back(QPointF(xmax, 0.0));
    else points_vec[idx] = QPointF(xmax , 0.0);

    if (idx < current_size) points_vec.resize(idx + 1);

    spectrum->blockSignals(true);
    line_series.blockSignals(true);

    line_series.replace(points_vec);

    spectrum->blockSignals(false);
    line_series.blockSignals(false);
}

void SpectrumWindow::scale_frequencies()
{
    const double scaling = freq_scaling->value() / 100.0;

    if (freq_scaling_type->currentIndex() == 0)
        for (int i = 0; i < scale_vec.size(); ++i)
            scale_vec[i] = scaling;
    else
        for (int i = 0; i < scale_vec.size(); ++i)
            scale_vec[i] = 1.0 - (1.0 - scaling) * frequencies[i] / frequencies[frequencies.size() - 1];
}

void SpectrumWindow::play_animations()
{
    QPropertyAnimation *anim3 = new QPropertyAnimation(spectrum_view, "geometry", this);
    anim3->setEasingCurve(QEasingCurve::OutBounce);
    anim3->setDuration(1500);
    anim3->setEndValue(spectrum_view->geometry());
    anim3->setStartValue(QRect(spectrum_view->x(), spectrum_view->y() - 200,
                         spectrum_view->width(), spectrum_view->height()));
    anim3->start(QAbstractAnimation::DeleteWhenStopped);
}

void SpectrumWindow::load_qsettings()
{
    const QString& keyfile = SystemSettings::keyfile;

    if (!QFileInfo::exists(keyfile))
        return;

    QSettings settings(keyfile, QSettings::IniFormat);
    settings.beginGroup("Spectrum");

    if (settings.contains("charttheme"))
        spectrum_view->set_theme(settings.value("charttheme").toInt());

    if (settings.contains("showtitle"))
        spectrum_view->set_title_visible(settings.value("showtitle").toBool());

    if (settings.contains("axistitlefont"))
        spectrum_view->set_axis_title_font(settings.value("axistitlefont").value<QFont>());

    if (settings.contains("axislabelfont"))
        spectrum_view->set_axis_label_font(settings.value("axislabelfont").value<QFont>());

    if (settings.contains("autocolour"))
        spectrum_view->set_auto_line_colour(settings.value("autocolour").toBool());

    if (settings.contains("linecolour"))
        spectrum_view->set_line_colour(settings.value("linecolour").value<QColor>());

    if (settings.contains("showtitle"))
        spectrum_view->set_title_visible(settings.value("showtitle").toBool());

    settings.endGroup();
}

void SpectrumWindow::write_key(const QString& key, const QVariant& value)
{
    const QString& keyfile = SystemSettings::keyfile;
    QSettings settings(keyfile, QSettings::IniFormat);

    settings.beginGroup("Spectrum");
    settings.setValue(key, value);
    settings.endGroup();
}

void SpectrumWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        hide();

    QWidget::keyPressEvent(event);
}

void SpectrumWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton
           && spectrum_view->hasFocus())
    {
        spectrum->zoomReset();
        update_plot();
        show_assignment_line();
        return;
    }

    QWidget::mousePressEvent(event);
}

