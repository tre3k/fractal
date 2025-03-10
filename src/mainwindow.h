/*
 *  Copyright (c) 2018-2025 Kirill Pshenichnyi
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
#include <QToolBar>
#include <QStatusBar>
#include <QProgressBar>

#include "about.h"
#include "iqcustomplot.h"
#include "functions.h"
#include "windowplot.h"
#include "config.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

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
		QAction *rescale_axis;
		QAction *close;
		QAction *show_about;

	} s_actions_;

	void buildMenuBar();
	void buildToolBar();
	void initActions();

	QStatusBar *status_bar_;
	struct s_status_bar {
		QProgressBar *progress_bar;
	} sbar_;
	void buildStatusBar();

	Data2D *data_input_;
	Data2D *data_fft_;
	Data2D *data_fft_phase_;
	Data2D *data_correlation_;

	iCasePlot2D *plot_input_;
	iCasePlot2D *plot_fft_;
	iCasePlot2D *plot_correlation_;

	WindowPlot *win_plot_;
	WindowPlot *win_plot_correlation_;
	Functions *funcs_;
	FFT2DThread *fft2d_thread_;
	CorrelationThread *correlation_thread_;

	double to_impulse_ = 1;

	QVector<double> *aver_x_, *aver_y_, *aver_err_;

	bool image_loaded_ = false;

	void plotData(iCasePlot2D *,Data2D *);
	void toCircle(
		double *x,
		double *y,
		double r,
		double phi
		);
	void paintCircles(iCasePlot2D *, double, double,
			  double, double, double, double, QColor);

	AboutDialog *about_dialog_;

private slots:
	void changeSpinBox(double);
	void slotChangeRangeFFT();

	void slotOpenText();
	void Average();
	void averageCorrelation();
	void slotOpenFFT();
	void slotRescale();
	void openImage(QString filename, Data2D *indata);
	void slotOpenImage();
	void slotOpenImageFFT();
	void invertData();
	void invertFFT();
	void gotoCenterMass();
	void buildFFT();
	void buildCorrelation();
	void completeCorrelation();

	void Close();

	void preProcess();

signals:
	void signal_plot(WindowPlotValues);
	void plotCorrelation(WindowPlotValues);

};

#endif // MAINWINDOW_H
