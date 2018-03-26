#ifndef WINDOWPLOT_H
#define WINDOWPLOT_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QVector>

#include "iqcustomplot.h"

struct windowPlotValues{
    QVector<double> *x,*y,*err;
    bool showError;
};

class windowPlot : public QMainWindow
{
    Q_OBJECT
public:
    explicit windowPlot(QWidget *parent = nullptr);

    QMenuBar *menuBar;
    QMenu *menuFile;
    QAction *actionSaveTxt;
    QAction *actionHideWindow;
    QMenu *menuScale;
    QAction *actionLogX;
    QAction *actionLogY;
    QAction *actionAutoscale;
    QMenu *menuApproximation;
    QAction *actionLinearApprox;


    iQCustomPlot *plot;

    windowPlotValues globalVal;

    void approximate();
    void linear_approx(double *,double *,double *,int,int,int,QVector<double> *,QVector<double> *);
    double linear_func(double,double,double,double);

signals:

public slots:
    void slot_plot(windowPlotValues);
    void slot_saveTxt();
    void slot_logX(bool);
    void slot_logY(bool);
    void slot_linearApprox(bool);
    void slot_autoscale();
    void slot_close();

};

#endif // WINDOWPLOT_H
