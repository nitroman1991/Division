cmake_minimum_required (VERSION 2.8)

project (Division)

find_package (Qt4 COMPONENTS QTCORE QTGUI QTOPENGL REQUIRED)
find_package (OpenGL REQUIRED)

set(QT_USE_QTOPENGL TRUE)

set (HEADERS ../subsidiary.h
			 ../Polygon.h
			 ../GLWidget.h)


			 
set (SOURCES ../main.cpp
			 ../subsidiary.cpp
			 ../Polygon.cpp
			 ../GLWidget.cpp)

			 
include (${QT_USE_FILE})

include_directories(${QT_INCLUDES} ${OPENGL_INCLUDE_DIRS})
			 
QT4_WRAP_CPP(main_MOC ${HEADERS})


add_executable(main ${SOURCES})

target_link_libraries (main ${QT_LIBRARIES} ${OPENGL_LIBRARIES})
