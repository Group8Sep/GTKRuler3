cmake_minimum_required(VERSION 3.16)
project(GTKRuler3)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)
add_library(GTKRuler3 gtkruler.c)


FIND_PACKAGE ( PkgConfig REQUIRED )
PKG_CHECK_MODULES( GTK REQUIRED gtk+-3.0 )
INCLUDE_DIRECTORIES ( ${GTK_INCLUDE_DIRS} )
target_link_libraries(GTKRuler3 ${GTK_LIBRARIES})
find_package(Cairo)
include_directories(${Cairo_INCLUDE_DIRS})
target_link_libraries(GTKRuler3 ${Cairo_LIBRARIES})
target_link_libraries(GTKRuler3 m)
