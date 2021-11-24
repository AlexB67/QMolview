#ifndef PREFSWINDOW_H
#define PREFSWINDOW_H

#include <QObject>
#include <QWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDial>
#include <QtWidgets/QPushButton>
#include <QEvent>

class PrefsWindow : public QWidget
{
    Q_OBJECT
public:
    explicit PrefsWindow(QWidget *parent = nullptr);
    void save_settings();
    void load_settings();

signals:
    void themePrefs_changed();
    void hideMenuBar_changed(QCheckBox *);
    void viewPortColour_changed(QColor viewport_colour);
    void iconColour_changed(bool, bool);

private:
    QGroupBox   *appearanceBox;
    QGroupBox   *miscBox;
    QGridLayout *appearancegrid;
    QGridLayout *miscgrid;
    QGridLayout *grid;
    QComboBox   *themeselect;
    QComboBox   *viewportcolour;
    QComboBox   *iconselect;
    QCheckBox   *standard_palette;
    QCheckBox   *hide_menu;
    QLabel      *theme_label;
    QLabel      *icon_label;
    QLabel      *viewport_label;

    QPushButton *restore_defaults;

    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *) override;

private slots:
    void changeTheme();
    void changeMenu();
    void changeViewPort();
    void changeIcons();
    void removeSettings();
};

#endif // PREFSWINDOW_H
