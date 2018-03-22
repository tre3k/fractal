#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <math.h>
#include <QVector>
#include <QDebug>

#ifndef M_PI
#define M_PI 4*atan(1)
#endif

class data2d{
public:
    int size_x,size_y;
    double **data;
    data2d(int,int);
    void remove();
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
};

#endif // FUNCTIONS_H
