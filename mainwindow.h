#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <math.h>

#include <QMainWindow>
#include <QImage>
#include <QRgb>

#include "iqcustomplot.h"
#include "functions.h"
#include "windowplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:
    void signal_plot(windowPlotValues);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slot_changeSpinBoxs(double);
    void slotChangeRangeFFT();


    void on_action_Open_triggered();

    void on_action_Close_triggered();

    void on_pushButtonIntegrate_clicked();

    void on_action_openFFT_triggered();

    void on_actionScale_triggered();

    void openImage(QString,data2d *);

    void on_actionOpenImage_triggered();

    void on_actionOpenImageFFT_triggered();

    void on_pushButton_invertData_clicked();

    void on_pushButton_invertFFT_clicked();

    void on_pushButtonCentre_clicked();

    void on_pushButton_FFT_clicked();

private:
    Ui::MainWindow *ui;

    data2d *data_input;
    data2d *data_fft;
    data2d *data_fft_phase;

    iCasePlot2D *plot_input;
    iCasePlot2D *plot_fft;
    iCasePlot2D *plot_fft_phase;

    windowPlot *winPlot;
    functions *funcs;

    double toImpulse = 1;

    QVector<double> *averX,*averY,*averErr;

    bool imageLoaded = false;

    void preProcess();
    void plotData(iCasePlot2D *,data2d *);
    void paintCircles(iCasePlot2D *,double,double,double,double);
};

#endif // MAINWINDOW_H
