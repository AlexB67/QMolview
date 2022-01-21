#ifndef SPECTRUMVIEW_H
#define SPECTRUMVIEW_H

#include <QtCharts>
#include <QChartView>
#include <QLineSeries>
#include <QEvent>
#include <QtWidgets/QLineEdit>

class SpectrumView : public QChartView
{
public:
    explicit SpectrumView(QLineEdit     *Xtracker,
                          QLineEdit     *Ytracker,
                          QChart        *spectrum,
                          QWidget       *parent = nullptr);
    ~SpectrumView() { m_spectrum->removeAllSeries(); delete m_spectrum; }

    void set_axes();
    void update_axes_values(qreal ymax, qreal xmin, qreal xmax,
                            bool x_reversed, bool y_reversed, bool update_ymax = true);

    void create_line_series();
    void add_line_series();
    void set_theme(int theme);
    void set_line_colour(const QColor&);
    void set_auto_line_colour(bool);
    void set_axis_title_font(const QFont&);
    void set_axis_label_font(const QFont&);
    void set_title_visible(bool);


    // For the unzoomed plot
    qreal axisY_min() const { return yaxis_min;}
    qreal axisY_max() const { return yaxis_max;}

    QLineSeries&  get_line_series() { return *m_line_series;}
    QValueAxis&   get_Yaxis() { return *Yaxis; }
    QValueAxis&   get_Yaxis_right() { return *Yaxis_right; }
    QValueAxis&   get_Xaxis() { return *Xaxis; }

private:
    QLineEdit      *m_Xtracker{nullptr};
    QLineEdit      *m_Ytracker{nullptr};
    QChart         *m_spectrum{nullptr};
    QLineSeries    *m_line_series{nullptr};
    QValueAxis     *Xaxis{nullptr};
    QValueAxis     *Yaxis{nullptr};
    QValueAxis     *Yaxis_right{nullptr};

    QFont axis_title_font;
    QFont axis_label_font;

    qreal yaxis_max{0};
    qreal yaxis_min{0};
    qreal xaxis_max{0};
    qreal xaxis_min{0};

    QColor current_line_colour;
    QColor default_line_colour;

    bool is_moving{false};
    bool auto_colour_line_series{true};

    void set_blend_theme();

    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // SPECTRUMVIEW_H
