cmake_minimum_required(VERSION 3.5)

# See also: https://cmake.org/cmake-tutorial/

project(GomerEngine LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set (GomerEngine_VERSION_MAJOR 0)
set (GomerEngine_VERSION_MINOR 0)
set (GomerEngine_VERSION_PATCH 1)

# configure a header file to pass some of the CMake settings
# to the source code, then add the binary tree to the search
# path for include files so that we will find config.h
configure_file    ( "${PROJECT_SOURCE_DIR}/config.h.in"
                    "${PROJECT_BINARY_DIR}/config.h" )
include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core)

add_executable(GomerEngine main.cpp
                           chiho.cpp
                           chiiki.cpp
                           console.cpp
                           draco.cpp
                           goban.cpp
                           goishi.cpp
                           gosu.cpp
                           gtphandler.cpp
                           jiyu.cpp
                           kigo.cpp
                           kogai.cpp
                           menseki.cpp
                           player.cpp
                           ruikei.cpp
                           ryoiki.cpp
                           sakudo.cpp
                           shiai.cpp
                           shiko.cpp
                           soshi.cpp
                           wyrm.cpp
              )

set(SOURCES config.h.in
            chiho.h
            chiiki.h
            console.h
            draco.h
            goban.h
            goishi.h
            gosu.h
            gtphandler.h
            jiyu.h
            kigo.h
            kogai.h
            menseki.h
            player.h
            ruikei.h
            ryoiki.h
            sakudo.h
            shiai.h
            shiko.h
            soshi.h
            wyrm.h
   )

target_link_libraries(GomerEngine Qt5::Core)
