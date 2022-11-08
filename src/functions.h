/*
 *  Copyright (c) 2018-2022 Kirill Pshenichnyi
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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QtMath>
#include <QVector>
#include <QDebug>

#ifndef M_PI
#define M_PI 4 * atan(1)
#endif

class data2d{
public:
    int size_x,size_y;
    double **data;
    data2d(int sx=0,int sy=0);
    void remove();
    void reinit(int, int);
};

class functions{
public:
    functions();
    void fft(double *,double *,int);
    void fft2d(double **,double **,int,int);
    void sort(double **,int,int);
    void makeFFT2D(data2d *,data2d *,data2d *);
    int doubleToInt(double);
    void toCircle(double *,double *,double,double);
    void average(data2d *,double,double,double,double,double,double,
                 QVector<double> *,QVector<double> *,QVector<double> *,bool,int);
    void inverteData(data2d *);
};

#endif // FUNCTIONS_H
