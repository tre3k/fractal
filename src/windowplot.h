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

#ifndef WINDOWPLOT_H
#define WINDOWPLOT_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QVector>

#include "iqcustomplot.h"

struct WindowPlotValues{
    QVector<double> *x, *y, *err;
    bool showError;
    bool logScale;
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
    QAction *actionDoubleLog;
    QAction *actionLogX;
    QAction *actionLogY;
    QAction *actionAutoscale;
    QMenu *menuApproximation;
    QAction *actionLinearApprox;


    iQCustomPlot *plot;

    WindowPlotValues global_val;

    void approximate();

signals:

public slots:
    void slot_plot(WindowPlotValues);
    void slot_saveTxt();
    void slot_doubleLog(bool);
    void slot_logX(bool);
    void slot_logY(bool);
    void slot_linearApprox(bool);
    void slot_autoscale();
    void slot_close();

};

#endif // WINDOWPLOT_H
