#
# Copyright (c) 2018-2025 Kirill Pshenichnyi <pshcyrill@mail.ru>
#
# This file is part of fractal.
#
# fractal is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# fractal is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with fractal.  If not, see <https://www.gnu.org/licenses/>.
#
#    Author: Kirill Pshenichnyi <pshcyrill@mail.ru>
#

cmake_minimum_required(VERSION 3.16)

set(PROGRAM_NAME fractal)
project(PROGRAM_NAME)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS PrintSupport REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Svg)

include_directories(
  ${PROJECT_SOURCE_DIR}/src/external
  ${PROJECT_SOURCE_DIR}/src
  )

execute_process(
  COMMAND "git" "describe" "--always" "--tags"
  OUTPUT_VARIABLE VERSION_GIT_OUTPUT
  )

string(TIMESTAMP TODAY "%d.%m.%Y")
string(TIMESTAMP COPY_YEAR "%Y")

string(REGEX REPLACE "\n$" "" VERSION_GIT_OUTPUT "${VERSION_GIT_OUTPUT}")
set(APPLICATION_VERSION \"${VERSION_GIT_OUTPUT}\")
set(APPLICATION_NAME "\"Fractal\"")

set(TODAY_TEXT \"${TODAY}\")
set(COPY_YEAR_TEXT \"${COPY_YEAR}\")
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
  src/about.cpp src/about.h
  src/config.h
  src/external/qcustomplot.cpp src/external/qcustomplot.h
  )

target_link_libraries(${PROGRAM_NAME}
  Qt6::Core
  Qt6::PrintSupport
  Qt6::Widgets
  Qt6::Svg
  )
