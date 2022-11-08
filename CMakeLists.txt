#
# Copyright (c) 2022 Kirill Pshenichnyi <pshcyrill@mail.ru>
#
# This file is part of choppers.
#
# choppers is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# choppers is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with choppers.  If not, see <https://www.gnu.org/licenses/>.
#
#    Author: Kirill Pshenichnyi <pshcyrill@mail.ru>
#

cmake_minimum_required(VERSION 3.16)

set(PROGRAM_NAME fractal)
project(PROGRAM_NAME)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Svg)

include_directories(
  ${PROJECT_SOURCE_DIR}/src/external
  ${PROJECT_SOURCE_DIR}/src
  )

execute_process(
  COMMAND "git" "describe" "--always" "--tags"
  OUTPUT_VARIABLE VERSION_GIT_OUTPUT
  )

string(TIMESTAMP TODAY "%d.%m.%Y")

string(REGEX REPLACE "\n$" "" VERSION_GIT_OUTPUT "${VERSION_GIT_OUTPUT}")
set(APPLICATION_VERSION \"${VERSION_GIT_OUTPUT}\")
set(APPLICATION_NAME "\"Fractal\"")

set(TODAY_TEXT \"${TODAY}\")
set(CMAKE_CXX_COMPILER_VERSION_TEXT \"${CMAKE_CXX_COMPILER_VERSION}\")
set(CMAKE_CXX_COMPILER_ID_TEXT \"${CMAKE_CXX_COMPILER_ID}\")

configure_file(config.h.in ../src/config.h)

add_executable(${PROGRAM_NAME}
  resources.qrc
  src/main.cpp
  src/mainwindow.cpp src/mainwindow.h
  src/iqcustomplot.cpp src/iqcustomplot.h
  src/functions.cpp src/functions.h
  src/windowplot.cpp src/windowplot.h
  src/external/qcustomplot.cpp src/external/qcustomplot.h
  )

target_link_libraries(${PROGRAM_NAME}
  Qt5::Core
  Qt5::PrintSupport
  Qt5::Widgets
  Qt5::Svg
  )
