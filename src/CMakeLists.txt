set(CMAKE_INCLUDE_CURRENT_DIR ON)

# where we install
SET(CMAKE_INSTALL_PREFIX /usr/local)

#Qt MOC and resource stuff
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# At some point if we want translations
#find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

# find_package(QT NAMES Qt6 Qt5 COMPONENTS 3DExtras 3DRender 3DCore Widgets REQUIRED)
# Uncomment the above and comment out bellow for qt6. 
# For now we use qt5, but qt6 builds.
find_package(QT NAMES Qt5 COMPONENTS 3DExtras 3DRender 3DCore Widgets REQUIRED)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS 3DExtras 3DRender 3DCore Widgets REQUIRED)
message(STATUS "Qt components found. Using " Qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR})

# set(TS_FILES qmolview_en_GB.ts)

set(PROJECT_SOURCES
        main.cpp
		fusionstyles.cpp
		geom.cpp
		mainwindow.cpp
		mainwindow_settings.cpp
		mainwindow_signals.cpp
		molview.cpp
		molview3D.cpp
		molview_signals.cpp
		prefswindow.cpp
		prefswindow_settings.cpp
		prefswindow_signals.cpp
		resources.qrc
        # ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qmolview MANUAL_FINALIZATION ${PROJECT_SOURCES})
    # qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    add_executable(qmolview ${PROJECT_SOURCES})
    # qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(qmolview PRIVATE Qt${QT_VERSION_MAJOR}::3DCore
                                       Qt${QT_VERSION_MAJOR}::3DRender
                                       Qt${QT_VERSION_MAJOR}::3DExtras
                                       Qt${QT_VERSION_MAJOR}::Widgets)

#set_target_properties(qmolview PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qmolview)
endif()

install(TARGETS qmolview DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/qmolview.desktop DESTINATION share/applications)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/qmolview.png DESTINATION share/icons/hicolor/512x512/apps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/qmolview.png DESTINATION share/icons/hicolor/256x256/apps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/qmolview.png DESTINATION share/icons/hicolor/128x128/apps)

message(INSTALL "Updating icon cache ...")
install(CODE "execute_process(COMMAND sudo gtk-update-icon-cache -qtf ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/)")

message(INSTALL "Updating desktop database ...")
install(CODE "execute_process(COMMAND sudo update-desktop-database -q ${CMAKE_INSTALL_PREFIX}/share/applications/)")

