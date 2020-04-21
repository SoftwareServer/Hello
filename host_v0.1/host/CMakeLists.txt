cmake_minimum_required(VERSION 2.8.12)
project(host)

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

find_package(Boost REQUIRED COMPONENTS
  system
  thread
)

#include_directories(include)

#aux_source_directory(src DIR_SRC)

add_executable(host 
  src/host.cpp
)

target_link_libraries(host
  ${Boost_THREAD_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
)
