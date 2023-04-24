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
	static void fft(double *, double *, int);
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

class FFTThread : public QThread {
	Q_OBJECT
public:
	enum Types {
		ROWS,
		COLUMNS
	};

	FFTThread();
	void setData(double **real, double **imgn, int N, int M);
	void setFromTo(int from, int to);
	void setType(Types type);

	void run();
	volatile bool isComplete();

private:
	volatile bool is_complete_ {false};
	int N_ {0}, M_ {0};
	double **real_ {nullptr}, **imgn_{nullptr};
	Types type_ {Types::ROWS};
	int from_, to_;

signals:
	void complete();
};

class FFT2DThread : public QThread {
	Q_OBJECT
public:
	FFT2DThread();
	void setData(Data2D *, Data2D *, Data2D *);

	void run();

private:
	Functions *funcs_;
	Data2D *data_in_ {nullptr};
	Data2D *data_out_ {nullptr};
	Data2D *data_out_phase_ {nullptr};
	bool data_is_loaded_ {false};

signals:
	void complete();
	void message(QString message, int timeout = 0);
};

class AverageThread : public QThread {
	Q_OBJECT
public:
	AverageThread();
	void setValues(Data2D *, double, double, double, double,
		       double, double, QVector<double> *,
		       QVector<double> *, QVector<double> *,
		       bool, int);
	void run();
signals:
	void complete();
	void progress(int);

};


#endif // FUNCTIONS_H
