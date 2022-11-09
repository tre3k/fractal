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
#include <QObject>
#include <QThread>

#ifndef M_PI
#define M_PI 4 * atan(1)
#endif

class Data2D{
public:
	int size_x,size_y;
	double **data;
	Data2D(int sx=0,int sy=0);
	void remove();
	void reinit(int, int);
};

class Functions{
public:
	Functions();
	void fft(double *, double *, int);
	void fft2d(double **, double **, int, int);
	void sort(double **, int, int);
	void makeFFT2D(Data2D *, Data2D *, Data2D *);
	int doubleToInt(double);
	void toCircle(double *, double *, double, double);
	void average(Data2D *, double, double, double, double, double, double,
		     QVector<double> *, QVector<double> *, QVector<double> *,
		     bool, int);
	void invertData(Data2D *);
};

class FFT2DThread : public QThread {
	Q_OBJECT
public:
	FFT2DThread();
	void setData(Data2D *, Data2D *, Data2D *);

	void run();
private:
	Functions *funcs_;
	Data2D * data_in_ {nullptr};
	Data2D * data_out_ {nullptr};
	Data2D * data_out_phase_ {nullptr};
	bool data_is_loaded_ {false};


signals:
	void isComplete();
};


#endif // FUNCTIONS_H
