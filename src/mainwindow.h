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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtMath>

#include <QMainWindow>
#include <QImage>
#include <QRgb>
#include <QAction>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScreen>
#include <QGuiApplication>

#include "iqcustomplot.h"
#include "functions.h"
#include "windowplot.h"
#include "config.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void signal_plot(windowPlotValues);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	QCheckBox *cb_size_of_pixel_;
	QCheckBox *cb_to_log_;

	QDoubleSpinBox *dsb_center_x_;
	QDoubleSpinBox *dsb_center_y_;
	QDoubleSpinBox *dsb_radius_in_;
	QDoubleSpinBox *dsb_radius_out_;
	QDoubleSpinBox *dsb_open_angle_;
	QDoubleSpinBox *dsb_position_angle_;
	QDoubleSpinBox *dsb_size_of_pixel_;

	QPushButton *pb_invert_data_;
	QPushButton *pb_invert_fft_;
	QPushButton *pb_fft_;
	QPushButton *pb_center_of_mass_;
	QPushButton *pb_average_;


	struct Actions {
		QAction *open_text;
		QAction *open_image;
		QAction *open_fft;
		QAction *open_image_fft;
		QAction *close;

	} s_actions_;

	void buildMenuBar();
	void buildToolBar();
	void initActions();


private slots:
	void changeSpinBox(double);
	void slotChangeRangeFFT();

	void slotOpenText();
	void on_pushButtonIntegrate_clicked();
	void on_action_openFFT_triggered();
	void on_actionScale_triggered();
	void openImage(QString filename, data2d *indata);
	void slotOpenImage();                          // with open dialog
	void on_actionOpenImageFFT_triggered();
	void on_pushButton_invertData_clicked();
	void on_pushButton_invertFFT_clicked();
	void on_pushButtonCentre_clicked();
	void on_pushButton_FFT_clicked();

	void Close();

private:
	data2d *data_input_;
	data2d *data_fft_;
	data2d *data_fft_phase_;

	iCasePlot2D *plot_input;
	iCasePlot2D *plot_fft;
	iCasePlot2D *plot_fft_phase;

	windowPlot *winPlot;
	functions *funcs;

	double toImpulse = 1;

	QVector<double> *averX, *averY, *averErr;

	bool imageLoaded = false;

	void preProcess();
	void plotData(iCasePlot2D *,data2d *);
	void toCircle(
		double *x,
		double *y,
		double r,
		double phi
		);
	void paintCircles(iCasePlot2D *, double, double,
			  double, double, double, double);
};

#endif // MAINWINDOW_H
