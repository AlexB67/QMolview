#ifndef SPECTRUMWINDOW_H
#define SPECTRUMWINDOW_H

#include "spectrum_view.h"
#include "qcustom/slider.h"
#include <QObject>
#include <QWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableWidget>
#include <QEvent>
#include <QTableWidget>
#include <QFont>
#include <QVector>
#include <QVariant>

using QCustom::Slider;

class SpectrumWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SpectrumWindow(QWidget *parent = nullptr);
    void play_animations();

public Q_SLOTS:
    void init_spectrum(const QVector<double>&, const QVector<double>&);
    void set_chart_theme(int);
    void set_plot_line_colour(const QColor&);
    void set_plot_auto_line_colour(bool);
    void set_plot_title_visible(bool);
    void set_plot_axis_title_font(const QFont&);
    void set_plot_axis_label_font(const QFont&);

private:
    QGroupBox      *box;
    QFrame         *spectrum_box;
    QGroupBox      *control_box;
    QGridLayout    *grid;
    QGridLayout    *spectrum_grid;
    QGridLayout    *control_grid;
    SpectrumView   *spectrum_view{nullptr};
    QChart         *spectrum{nullptr};
    QLineSeries    *assign_peak{nullptr};
    QLineEdit      *Xtracker;
    QLineEdit      *Ytracker;
    QLabel         *Xtracker_label;
    QLabel         *Ytracker_label;
    QLabel         *FWHH_label;
    QLabel         *freq_scaling_label;
    QLabel         *freq_scaling_type_label;
    QLabel         *line_shape_label;
    QDoubleSpinBox *FWHH_spinner;
    QDoubleSpinBox *freq_scaling;
    QComboBox      *freq_scaling_type;
    QCheckBox      *flip_Yaxis;
    QCheckBox      *flip_Xaxis;
    QCheckBox      *hide_selection;
    Slider         *line_shape_slider;
    QPushButton    *defaults;

    QTableWidget   *freq_table;
    QVector<QTableWidgetItem *> freq_Items;
    QVector<QTableWidgetItem *> intensity_Items;
    QVector<QTableWidgetItem *> absorp_Items;

    QVector<double> frequencies;
    QVector<double> intensities;
    QVector<double> scale_vec;
    QFont           table_font;

    QVector<QPointF> points_vec;

    bool is_moving{false}; // for mouse
    qreal ymax{0}; // for plot
    qreal xmin{0};
    qreal xmax{0};

    void write_key(const QString& key, const QVariant& value);
    void new_plot_points();
    void replace_plot_points();
    void update_freq_table();
    void create_freq_table();
    void init_freq_table();
    void scale_frequencies();
    void set_spectrum_defaults();
    void init_plot();
    void load_qsettings();

    void keyPressEvent(QKeyEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;

private Q_SLOTS:
     void update_plot();
     void update_plot_with_freq_scaling();
     void flip_Y_axis();
     void flip_X_axis();
     void show_assignment_line();
};


#endif // SPECTRUMWINDOW_H
