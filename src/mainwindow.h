#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcustom3D/molview3D.h"
#include "prefswindow.h"
#include "shortcuts_window.h"
#include "spectrum_window.h"
#include "qcustom/text_icon_button.h"
#include "qcustom/slider.h"
#include <QMainWindow>
#include <QObject>
#include <QEvent>
#include <QGuiApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDial>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QStatusBar>
#include <QtGui/QScreen>
#include <QEasingCurve>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
QT_END_NAMESPACE

using QCustom::TextIconButton;
using QCustom::Slider;

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
    QGroupBox   *viewBox;
    QGridLayout *grid;
    QGridLayout *ctrlgrid;
    TextIconButton *x_rotate;
    TextIconButton *y_rotate;
    TextIconButton *z_rotate;
    QPushButton *r_reset;
    QPushButton *end_frame;
    QPushButton *start_frame;
    QPushButton *start_mode_display;
    QPushButton *pause_mode_display;
    QPushButton *stop_mode_display;
    QPushButton *start_xyz_display;
    QPushButton *stop_xyz_display;
    QComboBox   *view_angles;
    QSpinBox    *frameCount;
    Slider      *animation_speed;
    Slider      *amplitude;
    QTableWidget    *normal_modes_table;
    QLabel      *mimeType;
    QLabel      *frameLabel;
    QLabel      *viewLabel;
    QLabel      *scaleAtomsLabel;
    QLabel      *placeholderComboText;
    QLabel      *animation_speed_label;
    QLabel      *amplitude_label;
    QLabel      *fileinfo_label;
    QLabel      *animinfo_label;
    QLabel      *caminfo_label;
    QDial       *set_atom_sizes;
    QCheckBox   *auto_mode_display;
    QCheckBox   *snap_to_centre;
    QCheckBox   *show_axes;
    QCheckBox   *show_norm_vectors;
    QMenu       *fileMenu;
    QMenu       *editMenu;
    QMenu       *viewMenu;
    QMenu       *helpMenu;
    QMenu       *hamBurgerMenu;
    QAction     *aboutAct;
    QAction     *aboutQtAct;
    QAction     *helpAct;
    QAction     *openAct;
    QAction     *saveAsAct;
    QAction     *exitAct;
    QAction     *prefsAct;
    QAction     *shortcutsAct;
    QAction     *viewSpectrumAct;
    QAction     *rotateleftAct;
    QAction     *rotaterightAct;
    QAction     *atomsScaleAct;
    QAction     *defaultAnimAct;
    QAction     *rotationModeAct;
    QToolBar    *toolbar;
    QToolBar    *toolbaredit;
    QToolBar    *toolbarhelp;
    QToolBar    *toolbarview;
    QToolBar    *toolbarctrl;
    QToolBar    *toolbarmenu;
    QToolButton *toolButton;

    // windows and other widgets
    MolView     *m_modifier{nullptr};
    PrefsWindow *prefswindow{nullptr};
    ShortCutsWindow *shortcutswindow{nullptr};
    SpectrumWindow *spectrumwindow{nullptr};

    // timers
    QTimer      *play_mode_timer{nullptr};
    QTimer      *play_xyz_timer{nullptr};
    QTimer      *x_rotate_timer{nullptr};
    QTimer      *y_rotate_timer{nullptr};
    QTimer      *z_rotate_timer{nullptr};

    float rotation_dir{1.0f};
    bool mode_display_paused{false};

    QString get_menu_style_sheet();
    void  setup_molview_signals_and_slots();
    void  create_menus_and_toolbar();
    QIcon set_icon_name(const QString& icon_name);
    QIcon set_icon_name(const QString& icon_name, const bool dark);
    void  play_widget_animations();
    void  set_viewport_colour();
    void  load_settings();

    bool eventFilter(QObject * obj, QEvent *event) override;
    void changeEvent(QEvent *) override;

private Q_SLOTS:
    void saveAsxyz();
    void sendX();
    void sendY();
    void sendZ();
    void openFileDialog();
    void sendView();
    void sendResetView();
    void sendFrameIndex();
    void editPrefs();
    void showShortcuts();
    void showSpectrum();
    void about();
    void sendsetFirstFrame();
    void sendsetLastFrame();
    void playAnimation();
    void stopAnimation();
    void sendplayNormalMode();
    void sendpauseNormalMode();
    void sendstopNormalMode();
    void setNormalModeAnimationSpeed();
    void sendAtomSizes();
    void activatePlay();
    void icon_theme_changed();
    void icon_theme_user_changed(bool);
    void icon_size_changed(int);
    void menubar_hide(QCheckBox *);
    void set_menubar_style(int);
    void set_dialog_style(bool);
    void viewport_colour_changed(QColor);
    void viewport_light_changed(int);
    void viewport_mouse_speed_changed(int);
    void populate_normal_modes_table(const QVector<double>&, const QVector<double>&);
    void rotateLeft();
    void rotateRight();
    void setDefaultAtomScale();
    void setDefaultAnimationParams();
    void showAxesChanged();
    void amplitudeChanged();
    void setXYZTimer();
    void setModeTimer();
    void setNormalModeVectors();
    void setXrotateTimer();
    void setYrotateTimer();
    void setZrotateTimer();
    void setRotationMode();
    void launchHelp();
};

#endif // MAINWINDOW_H
