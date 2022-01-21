set(CMAKE_INCLUDE_CURRENT_DIR ON)

#Qt MOC and resource stuff
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# At some point if we want translations
#find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

# For now we use qt5, but qt6 builds but has a point lights bug
# so wait till Qt fixes bug and we are ready for, just change 5 to 6 below

set(QT5_MINIMUM_VERSION "5.15.0")
set(QT6_MINIMUM_VERSION "6.2.0")

find_package(QT NAMES Qt5 COMPONENTS 3DExtras 3DRender 3DCore 3DInput Widgets Charts Xml REQUIRED)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS 3DExtras 3DRender 3DCore 3DInput Widgets Charts Xml REQUIRED)
message(STATUS "Qt components found. Using " Qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR})

# set(TS_FILES qmolview_en_GB.ts)

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/qcustom ${CMAKE_SOURCE_DIR}/qcustom3D)

set(PROJECT_SOURCES
        main.cpp
        qcustom/fusionstyles.cpp
        qcustom/text_icon_button.cpp
        qcustom/colour_button.cpp
        qcustom/slider.cpp
        qcustom3D/molview3D_atom.cpp
        qcustom3D/molview3D_bond.cpp
        qcustom3D/molview3D_axes.cpp
        qcustom3D/molview3D_cam_controller.cpp
        qcustom3D/molview3D.cpp
        molview.cpp
        molview_signals.cpp
        mainwindow.cpp
        mainwindow_settings.cpp
        mainwindow_signals.cpp
        prefswindow.cpp
        prefswindow_settings.cpp
        prefswindow_signals.cpp
        shortcuts_window.cpp
        spectrum_view.cpp
        spectrum_window.cpp
        spectrum_window_signals.cpp
        spectrum_window_freq_table.cpp
        molecule.cpp
        mol_utils.cpp
        cmlparser.cpp
        resources.qrc
        # ${TS_FILES}
)

if(UNIX)
	add_compile_options(-Wall -Wshadow -Wextra -Wpedantic -Wconversion)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qmolview MANUAL_FINALIZATION ${PROJECT_SOURCES})

    if (WIN32)
        set_property(TARGET qmolview PROPERTY WIN32_EXECUTABLE true)
    endif()

    # qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    add_executable(qmolview ${PROJECT_SOURCES})

    if (WIN32)
        set_property(TARGET qmolview PROPERTY WIN32_EXECUTABLE true)
    endif()

    # qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(qmolview  LINK_PUBLIC Qt${QT_VERSION_MAJOR}::3DCore
                                       Qt${QT_VERSION_MAJOR}::3DRender
                                       Qt${QT_VERSION_MAJOR}::3DExtras
                                       Qt${QT_VERSION_MAJOR}::3DInput
                                       Qt${QT_VERSION_MAJOR}::Widgets
                                       Qt${QT_VERSION_MAJOR}::Charts
                                       Qt${QT_VERSION_MAJOR}::Xml)

#set_target_properties(qmolview PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qmolview)
endif()

if (UNIX)
    set(CMAKE_INSTALL_MESSAGE LAZY)
    install(TARGETS qmolview DESTINATION bin)
    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/data/qmolview.desktop DESTINATION share/applications)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/org.physchem.qmolview.appdata.xml DESTINATION share/appdata)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/qmolview.png DESTINATION share/icons/hicolor/512x512/apps)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/qmolview.png DESTINATION share/icons/hicolor/256x256/apps)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/qmolview.png DESTINATION share/icons/hicolor/128x128/apps)
    install(CODE "execute_process(COMMAND sudo gtk-update-icon-cache -qtf ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/)")
    install(CODE "execute_process(COMMAND sudo update-desktop-database -q ${CMAKE_INSTALL_PREFIX}/share/applications/)")
endif()

