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

#include "windowplot.h"

windowPlot::windowPlot(QWidget *parent) : QMainWindow(parent)
{
	this->setMinimumWidth(600);
	this->setMinimumHeight(350);

	menuBar = new QMenuBar;
	menuFile = new QMenu(tr("&file"));
	actionSaveTxt = new QAction(tr("export to txt"), this);

	menuScale = new QMenu(tr("scale"));
	actionDoubleLog = new QAction(tr("double log."), this);
	actionDoubleLog->setCheckable(true);
	actionDoubleLog->setChecked(true);
	actionLogX = new QAction(tr("log. х"), this);
	actionLogX->setCheckable(true);
	actionLogX->setChecked(true);
	actionLogY = new QAction(tr("log. у"),this);
	actionLogY->setCheckable(true);
	actionLogY->setChecked(true);
	actionAutoscale = new QAction(tr("autoscale"), this);
	actionHideWindow = new QAction(tr("close"), this);

	menuApproximation = new QMenu(tr("approximate"), this);
	actionLinearApprox = new QAction(tr("linear approximation (LSM)"), this);
	actionLinearApprox->setCheckable(true);


	menuFile->addAction(actionSaveTxt);
	menuFile->addSeparator();
	menuFile->addAction(actionHideWindow);

	menuScale->addAction(actionDoubleLog);
	menuScale->addAction(actionLogX);
	menuScale->addAction(actionLogY);
	menuScale->addAction(actionAutoscale);

	menuApproximation->addAction(actionLinearApprox);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuScale);
	menuBar->addMenu(menuApproximation);

	this->setMenuBar(menuBar);

	plot = new iQCustomPlot;
	this->setCentralWidget(plot);

	connect(actionLogX,SIGNAL(toggled(bool)),
		this,SLOT(slot_logX(bool)));
	connect(actionLogY,SIGNAL(toggled(bool)),
		this,SLOT(slot_logY(bool)));
	connect(actionSaveTxt,SIGNAL(triggered(bool)),
		this,SLOT(slot_saveTxt()));
	connect(actionAutoscale,SIGNAL(triggered(bool)),
		this,SLOT(slot_autoscale()));
	connect(actionHideWindow,SIGNAL(triggered(bool)),
		this,SLOT(slot_close()));
	connect(actionLinearApprox,SIGNAL(toggled(bool)),
		this,SLOT(slot_linearApprox(bool)));
	connect(actionDoubleLog,SIGNAL(toggled(bool)),
		this,SLOT(slot_doubleLog(bool)));
}


void windowPlot::slot_plot(WindowPlotValues val){
	global_val = val;
	plot->clearGraphs();
	plot->addCurve(val.x, val.y, true, "black", "average");
	if(val.showError){
		QCPErrorBars *errorBar = new QCPErrorBars(
			plot->xAxis,plot->yAxis
			);
		errorBar->removeFromLegend();
		errorBar->setPen(QColor("black"));
		errorBar->addData(*val.err);
		errorBar->setDataPlottable(plot->graph(0));
		errorBar->selectionDecorator()->setPen(
			QPen(QColor("black"),
			     2,
			     Qt::SolidLine,
			     Qt::SquareCap,
			     Qt::BevelJoin)
			);
	}

	slot_logX(actionLogX->isChecked());
	slot_logY(actionLogY->isChecked());
	if(actionLinearApprox->isChecked()) approximate();
	plot->rescaleAxes(true);
	plot->xAxis->scaleRange(1.2);
	plot->yAxis->scaleRange(1.2);
	plot->replot();
}

void windowPlot::slot_doubleLog(bool val){
    actionLogX->setChecked(val);
    actionLogY->setChecked(val);
    slot_logX(val);
    slot_logY(val);
    slot_autoscale();
    return;
}

void windowPlot::slot_logX(bool val){
	if(val){
		plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
		plot->xAxis2->setScaleType(QCPAxis::stLogarithmic);
	}else{
		plot->xAxis->setScaleType(QCPAxis::stLinear);
		plot->xAxis2->setScaleType(QCPAxis::stLinear);
	}
	plot->replot();
}

void windowPlot::slot_logY(bool val){
	if(val){
		plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
		plot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
	}else{
		plot->yAxis->setScaleType(QCPAxis::stLinear);
		plot->yAxis2->setScaleType(QCPAxis::stLinear);
	}
	plot->replot();
}

void windowPlot::slot_saveTxt(){
	QString filename = QFileDialog::getSaveFileName(
		this,
		tr("Export data to txt"),
		"",
		"*.txt"
		);
	if(filename == "") return;
	if(filename.split('.').at(filename.split('.').size()-1) != "txt")
		filename += ".txt";
	QFile f(filename);
	if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
	QTextStream stream(&f);

	stream << "# x [pix]\ty\terr\n";
	for(int i=0;i<global_val.x->size();i++){
		if(global_val.showError){
			stream << global_val.x->at(i) << "\t"
			       << global_val.y->at(i) << "\t"
			       << global_val.err->at(i) << "\n";
		}else{
			stream << global_val.x->at(i) << "\t"
			       << global_val.y->at(i) << "\n";
		}
	}

	f.close();
}

void windowPlot::slot_autoscale(){
	plot->rescaleAxes(true);
	plot->xAxis->scaleRange(1.2);
	plot->yAxis->scaleRange(1.2);
	plot->replot();
}

void windowPlot::slot_linearApprox(bool val){
	if(!val){
		plot->clearItems();
		slot_plot(global_val);
	}else{
		approximate();
	}
}

void windowPlot::slot_close() {
	this->hide();
}

void windowPlot::approximate() {
	QVector<double> aX, aY;

	const int POINTS = 30;

	double a, b;
	double sx {0.0}, sx2 {0.0}, sy {0.0}, sxy {0.0};
	int n = global_val.x->size();

	for(int i = 0; i < n; i++) {
		sx += global_val.x->at(i);
		sx2 += global_val.x->at(i) * global_val.x->at(i);
		sy += global_val.y->at(i);
		sxy += global_val.y->at(i) * global_val.x->at(i);
	}

	a = (sxy - 1.0/n * sx * sy) / (sx2 - 1.0/n * sx * sx);
	b = 1.0/n * sy - a/n * sx;

	double di = (global_val.x->at(global_val.x->size() - 1) -
		     global_val.x->at(0)) / POINTS;

	for(double i = global_val.x->at(0);
	    i < global_val.x->at(global_val.x->size() - 1);
	    i += di) {
		aX.append(i);
		aY.append(a * i + b);
	}

	plot->clearItems();
        auto paramText = new QCPItemText(plot);
	paramText->setColor("red");
	paramText->position->setCoords(aX.at((aX.size() - 1) / 2), aY.at(2));
	paramText->setText(
		"a = " + QString::number(a) +
		"\nb = " + QString::number(b)
		);
	plot->addCurve(&aX, &aY, false, "red", "approximate");
}
