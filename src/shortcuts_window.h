#ifndef SHORTCUTSWINDOW_H
#define SHORTCUTSWINDOW_H

#include <QObject>
#include <QWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QEvent>
#include <QTableWidget>
#include <QFont>

class ShortCutsWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ShortCutsWindow(QWidget *parent = nullptr);

private:
    QGroupBox   *box;
    QGridLayout *grid;
    QGridLayout *shortcutsgrid;
    QTableWidget *shortcuts_table;
    QFont font;
    QFont font2;
    int count{0};

    void add_binding(const QString& binding, const QString& desc);

    void keyPressEvent(QKeyEvent *) override;

private slots:
};

#endif // SHORTCUTSWINDOW_H
