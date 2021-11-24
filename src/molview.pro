android|ios|tvos|winrt {
    warning( "Not supported on android, ios, tvos, or winrt." )
}

CONFIG += c++17
QT += 3dcore 3drender 3dextras widgets

SOURCES += main.cpp \
    fusionstyles.cpp \
    geom.cpp \
    mainwindow.cpp \
    mainwindow_settings.cpp \
    mainwindow_signals.cpp \
    molview.cpp \
    molview3D.cpp \
    molview_signals.cpp \
    prefswindow.cpp \
    prefswindow_settings.cpp \
    prefswindow_signals.cpp

HEADERS += \
    appglobals.h \
    fusionstyles.h \
    geom.h \
    mainwindow.h \
    molview.h \
    molview3D.h \
    prefswindow.h

RESOURCES += \
    resources.qrc

DISTFILES +=


