#include <cmakeconfig.h>
#include "appglobals.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char **argv)
{

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    // --platform xcb if on wayland
    QApplication::setOrganizationDomain("physchem.org");
    QApplication::setOrganizationName("physchem.org");
    QApplication::setApplicationName("qmoliew");
    QApplication::setApplicationVersion(VERSION);
    QGuiApplication::setDesktopFileName("org.physchem.qmolview");
    SystemSettings::install_preFix = QMOLVIEW_TARGET_DIR;

    MainWindow mainWin;
    mainWin.setWindowIcon(QIcon(":/icons/qmolview.png"));

#ifdef Q_OS_WIN
    app.setFont(QFont("Segoe UI"));
#endif

    mainWin.show();
    mainWin.resize(1100, 700);
    auto retcode = app.exec();
    return retcode;
}
