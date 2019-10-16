cmake_minimum_required(VERSION 3.5)

project(GomerEngine LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core)

add_executable(GomerEngine main.cpp
                           game.cpp       game.h
                           goban.cpp      goban.h
                           goishi.cpp     goishi.h
                           gosu.cpp       gosu.h
                           gtphandler.cpp gtphandler.h
                           player.cpp     player.h
                           shiko.cpp      shiko.h
                           wyrm.cpp       wyrm.h
              )
target_link_libraries(GomerEngine Qt5::Core)
