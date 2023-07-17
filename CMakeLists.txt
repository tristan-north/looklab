cmake_minimum_required(VERSION 3.24)
project(looklab)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
cmake_path(SET RMANTREE $ENV{RMANTREE})

include_directories(src)
include_directories(${IMATH_INCLUDEDIR})
include_directories(${ALEMBIC_INCLUDEDIR})
include_directories("${RMANTREE}include")

# Can't use debug -MDd in debug build as it's incompatible with prman lib.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_executable(looklab
        src/glwidget.cpp
        src/glwidget.h
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/shaders.h src/tif.cpp src/tif.h src/perftimer.h src/displaydriver.cpp src/displaydriver.h src/rman.cpp
        src/rman.h src/common.h src/renderview.cpp src/renderview.h src/geo.cpp src/geo.h
        src/parameters.cpp src/parameters.h src/nodegraph.cpp src/nodegraph.h src/ParamWidgets/sliderwidget.cpp src/ParamWidgets/sliderwidget.h)


target_link_libraries(looklab Qt5::Widgets)
target_link_libraries(looklab ${ALEMBIC_LIB})
target_link_libraries(looklab ${IMATH_LIB})
target_link_libraries(looklab
        "${RMANTREE}lib/libprman.lib"
        "${RMANTREE}lib/libpxrcore.lib"
        "${RMANTREE}lib/libstats.lib"
        "${RMANTREE}lib/libloadprman.lib"
        )
