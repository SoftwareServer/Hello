cmake_minimum_required(VERSION 2.8.12)
project(hostchart)

include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(--std=c++11)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml Quick QuickWidgets Qml Charts)
find_package(Boost REQUIRED COMPONENTS
  system
  thread
)

#include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
include_directories(${CMAKE_BINARY_DIR})


include_directories(include)

#aux_source_directory(src DIR_SRC)

##############################################################################
# Sections
##############################################################################
file(GLOB_RECURSE QT_MOC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS *.h)
#file(GLOB QT_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.ui)
#file(GLOB QT_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.qrc)

QT5_WRAP_CPP(QT_MOC_H ${QT_MOC})
#QT4_WRAP_UI(QT_FORMS_H ${QT_FORMS})
#QT4_ADD_RESOURCES(QT_RESOURCES_CPP ${QT_RESOURCES})

##############################################################################
# Sources
##############################################################################
#file(GLOB_RECURSE QT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS src/*.cpp)

#add_library(${PROJECT_NAME}
  #${QT_SOURCES}
  #${QT_RESOURCES_CPP}
  #${QT_FORMS_H}
  #${QT_MOC_H}
#)

add_executable(hostchart
  src/main.cpp
  #${QT_SOURCES}
  #${QT_RESOURCES_CPP} 
  #${QT_FORMS_H}
  ${QT_MOC_H}
)

target_link_libraries(hostchart
  ${Boost_THREAD_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${QT_LIBRARIES}
  Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml Qt5::Qml Qt5::Quick Qt5::QuickWidgets Qt5::Charts
)
