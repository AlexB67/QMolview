cmake_minimum_required (VERSION 3.5)

#  Qt creator doesn't pick this up
SET($(PROJECT_NAME) "qmolview")
SET(LANGUAGES CXX)
# for Qt creator
project ("qmolview")

# where we install
# SET(CMAKE_INSTALL_PREFIX /usr/local)

# config file
SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "6")
SET(VERSION_MICRO "0")
SET(QMOLVIEW_TARGET_DIR ${CMAKE_INSTALL_PREFIX})
configure_file(qmolview_cmake_config.h.in cmakeconfig.h )


# Enable release build
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_subdirectory(src)
add_subdirectory(extras)
