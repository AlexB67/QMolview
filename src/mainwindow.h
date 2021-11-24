#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "molview3D.h"
#include "prefswindow.h"
#include <QMainWindow>
#include <QObject>
#include <QGuiApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QEvent>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDial>
#include <QtWidgets/QTableWidget>
#include <QtGui/QScreen>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QThread>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
  //  ~MainWindow() override {delete view;}

private:
    // widgets 'n' layouts
    MolView3D   *view;
    QWidget     *container;
    QHBoxLayout *viewLayout;
    QGroupBox   *ctrlBox;
    QGroupBox   *fileBox;
    QGroupBox   *viewBox;
    QGridLayout *ctrlgrid;
    QGridLayout *filegrid;
    QPushButton *x_rotate;
    QPushButton *y_rotate;
    QPushButton *z_rotate;
    QPushButton *r_reset;
    QPushButton *end_frame;
    QPushButton *start_frame;
    QPushButton *start_mode_display;
    QPushButton *stop_mode_display;
    QPushButton *start_xyz_display;
    QPushButton *stop_xyz_display;
    QPushButton *atom_size_default;
    QComboBox   *view_angles;
    QSpinBox    *frameCount;
    QTableWidget    *normal_modes_table;
    QLabel      *numFrames;
    QLabel      *mimeType;
    QLabel      *frameLabel;
    QLabel      *viewLabel;
    QLabel      *scaleAtomsLabel;
    QDial       *set_atom_sizes;
    QCheckBox   *auto_mode_display;
    QMenu       *fileMenu;
    QMenu       *editMenu;
    QMenu       *helpMenu;
    QAction     *aboutAct;
    QAction     *aboutQtAct;
    QAction     *openAct;
    QAction     *exitAct;
    QAction     *prefsAct;
    QToolBar    *toolbar;
    QToolBar    *toolbaredit;
    MolView     *m_modifier{nullptr};
    PrefsWindow *prefswindow{nullptr};
    QTimer      *play_mode_timer{nullptr};
    QTimer      *play_xyz_timer{nullptr};

    void  setup_molview_signals_and_slots();
    void  create_menus_and_toolbar();
    bool  is_dark_theme();
    QIcon set_icon_name(const QString& icon_name);
    QIcon set_icon_name(const QString& icon_name, const bool dark);
    void  play_widget_animations();
    void  set_viewport_colour();
    void  load_settings();

private slots:
    void sendX();
    void sendY();
    void sendZ();
    void openFileDialog();
    void sendView();
    void sendResetView();
    void sendFrameIndex();
    void editPrefs();
    void about();
    void sendsetFirstFrame();
    void sendsetLastFrame();
    void playAnimation();
    void stopAnimation();
    void sendplayNormalMode();
    void sendstopNormalMode();
    void sendAtomSizes();
    void activatePlay();
    void icon_theme_changed();
    void icon_theme_user_changed(bool);
    void menubar_hide(QCheckBox *);
    void viewport_colour_changed(QColor colour);
    void populate_normal_modes_table(const QVector<double>&, const QVector<double>&);
};

#endif // MAINWINDOW_H
