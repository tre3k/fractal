/*
 *  Copyright (c) 2018-2023 Kirill Pshenichnyi
 *
 *  This file is part of fractal.
 *
 *  fractal is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  fractal is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with fractal.  If not, see <https://www.gnu.org/licenses/>.
 *
 *     Author: Kirill Pshenichnyi <pshcyrill@mail.ru>
 */

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QVector>
#include <QDir>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QVector<QString> translations_paths = {
	    "langs/",
	    "/usr/share/fractal/langs/",
	    QDir::homePath() + ".fractal/langs/",
    };

    for(auto path : translations_paths) {
	    if(translator.load(
		       QDir(path +
			    "fractal_" +
			    QLocale::system().name()).absolutePath())
		    ) {
		    a.installTranslator(&translator);
		    break;
	    }
    }

    MainWindow w;
    w.show();

    return a.exec();
}
