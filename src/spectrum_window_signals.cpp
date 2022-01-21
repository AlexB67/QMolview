#include "spectrum_window.h"
#include "appglobals.h"

void SpectrumWindow::init_plot()
{
    spectrum->removeAllSeries();
    if (!intensities.size()) return;

    assign_peak = nullptr;
    spectrum_view->create_line_series();
    new_plot_points();
    spectrum_view->add_line_series();

    QLineSeries& line_series = spectrum_view->get_line_series();

    bool is_dark = SystemSettings::theme_is_dark();

    QPen pen;
    (is_dark) ? pen.setColor(QColor(153, 210, 255))
              : pen.setColor(QColor(10, 10, 10));
    line_series.setPen(pen);

    spectrum->legend()->hide();
    spectrum_view->set_axes();
    spectrum_view->update_axes_values(ymax, xmin, xmax, flip_Xaxis->isChecked(), flip_Yaxis->isChecked());

    // peak asignment line
    assign_peak = new QLineSeries;

    assign_peak->blockSignals(true);
    auto& y_axis = spectrum_view->get_Yaxis();
    auto& x_axis = spectrum_view->get_Xaxis();

    // fill in dummies
    assign_peak->append(0, 0);
    assign_peak->append(0, 0);

    QPen line_pen;
    line_pen.setWidthF(0.75);
    line_pen.setColor(qApp->palette().brush(QPalette::Text).color());
    assign_peak->setPen(line_pen);

    spectrum->addSeries(assign_peak);

    assign_peak->attachAxis(&x_axis);
    assign_peak->attachAxis(&y_axis);
    assign_peak->hide();
    assign_peak->blockSignals(false);
}

void SpectrumWindow::update_plot()
{
    replace_plot_points();
    Q_EMIT spectrum_view->get_line_series().pointsReplaced();

    if (spectrum->isZoomed())
       spectrum_view->update_axes_values(spectrum_view->get_Yaxis().max(),
                                         spectrum_view->get_Xaxis().min(),
                                         spectrum_view->get_Xaxis().max(),
                                         flip_Xaxis->isChecked(), flip_Yaxis->isChecked(), false);
    else
       spectrum_view->update_axes_values(ymax, xmin, xmax, flip_Xaxis->isChecked(), flip_Yaxis->isChecked());

    update_freq_table();
}

void SpectrumWindow::update_plot_with_freq_scaling()
{
    if (spectrum->isZoomed()) spectrum->zoomReset();
    scale_frequencies();
    replace_plot_points();
    Q_EMIT spectrum_view->get_line_series().pointsReplaced();

    if (spectrum->isZoomed())
       spectrum_view->update_axes_values(spectrum_view->get_Yaxis().max(),
                                         spectrum_view->get_Xaxis().min(),
                                         spectrum_view->get_Xaxis().max(),
                                         flip_Xaxis->isChecked(), flip_Yaxis->isChecked(), false);
    else
       spectrum_view->update_axes_values(ymax, xmin, xmax, flip_Xaxis->isChecked(), flip_Yaxis->isChecked());

    update_freq_table();
}

void SpectrumWindow::flip_Y_axis()
{
    QValueAxis& yaxis = spectrum_view->get_Yaxis();
    yaxis.setReverse(flip_Yaxis->isChecked());
    show_assignment_line();
}

void SpectrumWindow::flip_X_axis()
{
    QValueAxis& xaxis = spectrum_view->get_Xaxis();
    xaxis.setReverse(flip_Xaxis->isChecked());
    show_assignment_line();
}

void SpectrumWindow::init_spectrum(const QVector<double>& freq, const QVector<double>& ir_ints)
{
    if (!ir_ints.size())
    {
        setVisible(false);
        return;
    }

    intensities = ir_ints;
    frequencies = freq;
    scale_vec = frequencies;
    set_spectrum_defaults();
    init_freq_table();
    init_plot();
    load_qsettings();
}

void SpectrumWindow::set_spectrum_defaults()
{
    flip_Yaxis->blockSignals(true);
    flip_Yaxis->setChecked(false);
    flip_Yaxis->blockSignals(false);

    flip_Xaxis->blockSignals(true);
    flip_Xaxis->setChecked(true);
    flip_Xaxis->blockSignals(false);

    hide_selection->blockSignals(true);
    hide_selection->setChecked(false);
    hide_selection->blockSignals(false);

    freq_scaling_type->blockSignals(true);
    freq_scaling_type->setCurrentIndex(0);
    freq_scaling_type->blockSignals(false);

    freq_scaling->blockSignals(true);
    freq_scaling->setValue(100.0);
    freq_scaling->blockSignals(false);

    FWHH_spinner->blockSignals(true);
    FWHH_spinner->setValue(12.0);
    FWHH_spinner->blockSignals(false);

    line_shape_slider->blockSignals(true);
    line_shape_slider->setValue(0);
    line_shape_slider->blockSignals(false);

    scale_frequencies(); // generate freq scale vector
}

void SpectrumWindow::show_assignment_line()
{
    if (hide_selection->isChecked())
    {
        if (assign_peak) assign_peak->hide();
        freq_table->clearSelection();
        freq_table->setSelectionMode(QAbstractItemView::NoSelection);
        return;
    }

    freq_table->setSelectionMode(QAbstractItemView::SingleSelection);

    int row = freq_table->currentRow();
    if (row < 0 || row >= freq_table->rowCount()) return;

    assign_peak->replace(0, frequencies[row] * scale_vec[row], spectrum_view->axisY_min());
    assign_peak->replace(1, frequencies[row] * scale_vec[row], spectrum_view->axisY_max());

    Q_EMIT assign_peak->pointsReplaced();

    assign_peak->show();
}

void SpectrumWindow::set_chart_theme(int theme)
{
    write_key("charttheme", theme);
    spectrum_view->set_theme(theme);

    if (assign_peak)
    {
        auto line_pen = assign_peak->pen();
        line_pen.setWidthF(0.75);
        line_pen.setColor(qApp->palette().brush(QPalette::Text).color());
        assign_peak->setPen(line_pen);
    }
}

void SpectrumWindow::set_plot_axis_title_font(const QFont& font)
{
    write_key("axistitlefont", font);
    spectrum_view->set_axis_title_font(font);
}

void SpectrumWindow::set_plot_axis_label_font(const QFont& font)
{
    write_key("axislabelfont", font);
    spectrum_view->set_axis_label_font(font);
}

void SpectrumWindow::set_plot_line_colour(const QColor& colour)
{
    write_key("linecolour", colour);
    spectrum_view->set_line_colour(colour);
}

void SpectrumWindow::set_plot_auto_line_colour(bool auto_colour)
{
    write_key("autocolour", auto_colour);
    spectrum_view->set_auto_line_colour(auto_colour);
}

void SpectrumWindow::set_plot_title_visible(bool show)
{
    write_key("showtitle", show);
    spectrum_view->set_title_visible(show);
}






