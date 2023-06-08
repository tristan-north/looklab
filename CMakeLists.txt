cmake_minimum_required(VERSION 3.24)
project(looklab)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)

include_directories(src)
include_directories(${IMATH_INCLUDEDIR})
include_directories(${ALEMBIC_INCLUDEDIR})

add_executable(looklab
        src/glwidget.cpp
        src/glwidget.h
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mesh.cpp
        src/mesh.h
        src/shaders.h src/tif.cpp src/tif.h)


target_link_libraries(looklab Qt5::Widgets)
target_link_libraries(looklab ${ALEMBIC_LIB})
target_link_libraries(looklab ${IMATH_LIB})