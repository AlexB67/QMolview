#include "appglobals.h"
#include "spectrum_view.h"

SpectrumView::SpectrumView(QLineEdit    *Xtracker,
                           QLineEdit    *Ytracker,
                           QChart       *spectrum,
                           QWidget      *parent)

 : QChartView(spectrum, parent),
   m_Xtracker(Xtracker),
   m_Ytracker(Ytracker),
   m_spectrum(spectrum)
{
    m_spectrum->setAnimationOptions(QChart::NoAnimation);
    setRenderHint(QPainter::Antialiasing);
    setRubberBand(QChartView::RectangleRubberBand);

#ifdef Q_OS_LINUX
    axis_title_font = QFont("Nimbus Roman", 14);
    axis_label_font = QFont("Nimbus Roman", 11);
#else
    axis_title_font = QFont("Times New Roman", 14);
    axis_label_font = QFont("Times New Roman", 11);
#endif

    (SystemSettings::theme_is_dark())
    ? current_line_colour = QColor(153, 210, 255)
    : current_line_colour = QColor(10, 10, 10);

    default_line_colour = current_line_colour;
}

void SpectrumView::create_line_series()
{
    m_line_series = new QLineSeries();
}

void SpectrumView::add_line_series()
{
    m_spectrum->addSeries(m_line_series);
}

void SpectrumView::mouseReleaseEvent(QMouseEvent *event)
{
    if (Qt::RightButton == event->button())
    {
       chart()->zoomReset();
       return;
    }
    else if (Qt::LeftButton == event->button())
    {
        setCursor(Qt::CursorShape::CrossCursor);
    }

    QChartView::mouseReleaseEvent(event);

}

void SpectrumView::mousePressEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button())
    {
        qreal xVal = m_spectrum->mapToValue(event->pos()).x();
        qreal yVal = m_spectrum->mapToValue(event->pos()).y();

        if (xVal > Xaxis->min() && xVal < Xaxis->max() &&
            yVal > Yaxis->min() && yVal < Yaxis->max())
            setCursor(Qt::ClosedHandCursor);
    }

    QChartView::mousePressEvent(event);

}

void SpectrumView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_Xtracker && m_Ytracker)
    {
        qreal xVal = m_spectrum->mapToValue(event->pos()).x();
        qreal yVal = m_spectrum->mapToValue(event->pos()).y();

        if (xVal > Xaxis->min() && xVal < Xaxis->max() &&
            yVal > Yaxis->min() && yVal < Yaxis->max())
        {
            if (!is_moving)
            {
                setCursor(Qt::CursorShape::CrossCursor);
                is_moving = true;
            }

            m_Xtracker->setText(QString::number(xVal, 'f', 4));
            m_Ytracker->setText(QString::number(yVal, 'f', 4));
        }
        else
        {
            m_Xtracker->clear();
            m_Ytracker->clear();

            if (is_moving)
            {
                unsetCursor();
                is_moving = false;
            }

        }
    }

    QChartView::mouseMoveEvent(event);
}

void SpectrumView::set_axes()
{
    if (Xaxis)
    {
        m_spectrum->removeAxis(Xaxis);
        delete Xaxis;
        Xaxis = nullptr;
    }

    if (Yaxis)
    {
        m_spectrum->removeAxis(Yaxis);
        delete Yaxis;
        Yaxis = nullptr;
    }

    if (Yaxis_right)
    {
        m_spectrum->removeAxis(Yaxis_right);
        delete Yaxis_right;
        Yaxis_right = nullptr;
    }

    Xaxis = new QValueAxis;
    Yaxis = new QValueAxis;
    Yaxis_right = new QValueAxis;

    set_theme(8);

    m_spectrum->addAxis(Xaxis, Qt::AlignBottom);
    m_spectrum->addAxis(Yaxis, Qt::AlignLeft);
    m_spectrum->addAxis(Yaxis_right, Qt::AlignRight);

    m_line_series->attachAxis(Xaxis);
    m_line_series->attachAxis(Yaxis);
    m_line_series->attachAxis(Yaxis_right);
}
// Take the maximum data point and convert it to an axis maximum
// to leave nice axis numbers
// we can optionally not update the axis maximum (for zoom scenarios)
// update is true by default
void SpectrumView::update_axes_values(qreal ymax, qreal xmin, qreal xmax,
                                      bool x_reversed, bool y_reversed, bool update_axes)
{
    if (!Yaxis) return;

    yaxis_max = std::ceil(1.125 * ymax);
    yaxis_min = - yaxis_max / 20.0;
    xaxis_min = xmin;
    xaxis_max = xmax;

    if (update_axes)
    {
        Yaxis->setMax(yaxis_max);
        Xaxis->setMax(xaxis_max);

        Yaxis->setMin(yaxis_min);
        Xaxis->setMin(xmin);
    }

    if (yaxis_max > 999.99)
    {
        Yaxis->setLabelFormat("%6.1f");
        Yaxis_right->setLabelFormat("%6.1f");
    }
    else
    {
        Yaxis->setLabelFormat("%6.2f");
        Yaxis_right->setLabelFormat("%6.2f");
    }


    Xaxis->setReverse(x_reversed);
    Yaxis->setReverse(y_reversed);
    // Yaxis_right will follow suit automatically
}

void SpectrumView::set_blend_theme()
{
    if (!Xaxis) return; // we are not in business yet

    QPalette palette = qApp->palette();
    QColor colour = palette.base().color();
    QBrush brush = palette.brush(QPalette::Window);
    brush.setColor(colour);
    m_spectrum->setBackgroundBrush(brush);
    m_spectrum->setBackgroundRoundness(4);

    QPen pen;
    pen.setColor(Qt::black);

    bool is_dark = SystemSettings::theme_is_dark();
    (is_dark ) ? pen.setWidthF(0.30) : pen.setWidthF(0.20);
    m_spectrum->setBackgroundPen(pen);

    m_spectrum->setPlotAreaBackgroundBrush(brush);
    Xaxis->setLinePen(palette.windowText().color());
    Yaxis->setLinePen(palette.windowText().color());
    Yaxis_right->setLinePen(palette.windowText().color());
    Xaxis->setLinePenColor(palette.windowText().color());
    Yaxis->setLinePenColor(palette.windowText().color());
    Yaxis_right->setLinePenColor(palette.windowText().color());
    brush = palette.brush(QPalette::WindowText);
    Xaxis->setLabelsColor(palette.windowText().color());
    Yaxis->setLabelsColor(palette.windowText().color());
    Yaxis_right->setLabelsColor(palette.windowText().color());
    m_spectrum->setTitleBrush(brush);
    Xaxis->setTitleBrush(brush);
    Yaxis->setTitleBrush(brush);
    Yaxis_right->setTitleBrush(brush);

    QPen pen_line;
   // pen_line.setWidth(1);
    (is_dark) ? pen_line.setColor(QColor(153, 210, 255))
              : pen_line.setColor(QColor(10, 10, 10));

    m_line_series->setPen(pen_line);
}

void SpectrumView::set_theme(int theme)
{
    if (!Xaxis) return; // we are not in business yet

    setRenderHint(QPainter::Antialiasing);

    if (theme > 7) set_blend_theme();
    else
    {
        m_line_series->setPen(Qt::NoPen); // clear blend theme pen override
        m_spectrum->setTheme(static_cast<QChart::ChartTheme>(theme));
    }

    default_line_colour = m_line_series->color();

    Xaxis->setTitleFont(axis_title_font);
    Yaxis->setTitleFont(axis_title_font);
    Xaxis->setTitleText("<i>&nu;</i> / cm<sup> -1</sup>");
    Yaxis->setTitleText("<i>&epsilon;</i> / L mol <sup> -1</sup> cm<sup> -1</sup>");
    Xaxis->setLabelsFont(axis_label_font);
    Yaxis->setLabelsFont(axis_label_font);

    QPen newpen;
    (theme > 7) ? newpen.setWidthF(0.25) : newpen.setWidthF(0.5);
    newpen.setStyle(Qt::DashLine);

    (theme > 7) ? newpen.setColor(qApp->palette().brush(QPalette::WindowText).color())
                : newpen.setColor(Yaxis->gridLinePen().color());

    Xaxis->setGridLinePen(newpen);
    Xaxis->setTickCount(9);
    Yaxis->setGridLinePen(newpen);

    Yaxis->setLabelFormat("%6.2f");
    Yaxis_right->setLabelsFont(axis_label_font);

    QPen newpen2;
    newpen2.setWidthF(1.0);
    newpen2.setStyle(Qt::SolidLine);

    (theme > 7) ? newpen2.setColor(qApp->palette().brush(QPalette::WindowText).color())
                : newpen2.setColor(Yaxis->labelsColor());

    Yaxis_right->setTickCount(0);
    Yaxis_right->setLinePen(newpen2);

    Yaxis_right->setGridLineVisible(false);
    Yaxis_right->setLabelFormat("%6.2f");
}

void SpectrumView::set_line_colour(const QColor& colour)
{
    if (!Xaxis) return;

    if (!auto_colour_line_series)
    {
        current_line_colour = colour;
        m_line_series->setColor(current_line_colour);
    }
}

void SpectrumView::set_auto_line_colour(bool auto_colour)
{
    auto_colour_line_series = auto_colour;

    if (!Xaxis) return;

    if (auto_colour)
        m_line_series->setColor(default_line_colour);
    else
        m_line_series->setColor(current_line_colour);
}

void SpectrumView::set_axis_title_font(const QFont& font)
{
    if (!Xaxis) return;

    axis_title_font = font;
    Xaxis->setTitleFont(font);
    Yaxis->setTitleFont(font);
    Yaxis_right->setTitleFont(font);

}

void SpectrumView::set_axis_label_font(const QFont& font)
{
    if (!Xaxis) return;

    axis_label_font = font;
    Xaxis->setLabelsFont(font);
    Yaxis->setLabelsFont(font);
    Yaxis_right->setLabelsFont(font);
}

void SpectrumView::set_title_visible(bool show)
{
    (show) ? m_spectrum->setTitle(tr("IR spectrum")) :
             m_spectrum->setTitle(tr(""));
}
