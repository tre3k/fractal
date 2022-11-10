#
# Copyright (c) 2018 - 2022 Kirill Pshenichnyi <pshcyrill@mail.ru>
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

QT += core gui widgets printsupport

QTPLUGIN += qsvg

TARGET = fractal
TEMPLATE = app


SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
        src/iqcustomplot.cpp \
        src/external/qcustomplot.cpp \
        src/functions.cpp \
        src/windowplot.cpp \
        src/about.cpp

HEADERS += \
        src/mainwindow.h \
        src/iqcustomplot.h \
        src/external/qcustomplot.h \
        src/functions.h \
        src/windowplot.h \
        src/about.h \
        src/config.h

INCLUDEPATH += \
        src/ \
        src/external

RESOURCES += resources.qrc
TRANSLATIONS += langs/fractal_ru.ts
