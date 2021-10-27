/*
 *  Copyright (c) 2018-2021 NRC KI PNPI, Gatchina, LO, 188300 Russia
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
    menuFile = new QMenu("&file");
    actionSaveTxt = new QAction("export to txt",this);

    menuScale = new QMenu("scale");
    actionDoubleLog = new QAction("double log.",this);
    actionDoubleLog->setCheckable(true);
    actionDoubleLog->setChecked(true);
    actionLogX = new QAction("log. х",this);
    actionLogX->setCheckable(true);
    actionLogX->setChecked(true);
    actionLogY = new QAction("log. у",this);
    actionLogY->setCheckable(true);
    actionLogY->setChecked(true);
    actionAutoscale = new QAction("autoscale.",this);
    actionHideWindow = new QAction("close", this);

    menuApproximation = new QMenu("approximate (experemental)",this);
    actionLinearApprox = new QAction("power approximation",this);
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


void windowPlot::slot_plot(windowPlotValues val){
    globalVal = val;
    plot->clearGraphs();
    plot->addCurve(val.x,val.y,true,"black","average");
    if(val.showError){
        QCPErrorBars *errorBar = new QCPErrorBars(plot->xAxis,plot->yAxis);
        errorBar->removeFromLegend();
        errorBar->setPen(QColor("black"));
        errorBar->addData(*val.err);
        errorBar->setDataPlottable(plot->graph(0));
        errorBar->selectionDecorator()->setPen(QPen(QColor("black"),
                  2,Qt::SolidLine,Qt::SquareCap,Qt::BevelJoin));
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
    QString filename = QFileDialog::getSaveFileName(this,"Export data to txt","","*.txt");
    if(filename=="") return;
    if(filename.split('.').at(filename.split('.').size()-1)!="txt") filename += ".txt";
    QFile f(filename);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream stream(&f);

    stream << "# x [pix]\ty\terr\n";
    for(int i=0;i<globalVal.x->size();i++){
        if(globalVal.showError){
            stream << globalVal.x->at(i) << "\t"
                   << globalVal.y->at(i) << "\t"
                   << globalVal.err->at(i) << "\n";
        }else{
            stream << globalVal.x->at(i) << "\t"
                   << globalVal.y->at(i) << "\n";
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
        slot_plot(globalVal);
    }else{
        approximate();
    }
}

void windowPlot::slot_close(){
    this->hide();
}

void windowPlot::approximate(){
    double xMax,xMin,yMax,yMin;
    QVector<double> aX,aY;
    //QVector<double> medialX,medialY;
    //double wind = 4,S=0;
    double daX;

    double pA,pD,pB;  //Amplitude step and background pA*x^-pD+pB

    /*
    for(int i=0;i<globalVal.x->size()-wind;i++){
        S=0;
        for(int j=0;j<wind;j++){
            S+=globalVal.y->at(i+j)/wind;
        }
        medialY.append(S);
        medialX.append(globalVal.x->at(i+wind/2));
    }
    */

    xMin = globalVal.x->at(0);
    //xMin = medialX.at(0);
    xMax = xMin;
    yMin = globalVal.y->at(0);
    //xMin = medialY.at(0);
    yMax = yMin;

    for(int i=0;i<globalVal.x->size();i++){
        if(globalVal.x->at(i) < xMin) xMin = globalVal.x->at(i);
        if(globalVal.x->at(i) > xMax) xMax = globalVal.x->at(i);
        if(globalVal.y->at(i) < yMin) yMin = globalVal.y->at(i);
        if(globalVal.y->at(i) > yMax) yMax = globalVal.y->at(i);
    }

    /*
    for(int i=0;i<globalVal.x->size();i++){
        if(medialX.at(i) < xMin) xMin = medialX.at(i);
        if(medialX.at(i) > xMax) xMax = medialX.at(i);
        if(medialY.at(i) < yMin) yMin = medialY.at(i);
        if(medialY.at(i) > yMax) yMax = medialY.at(i);
    }
    */

    pD = 1.0;
    pB = yMin;
    pA = globalVal.y->at(2);

    for(int i=0;i<30;i++){
        //qDebug () << "A = " << pA << "; D = " << pD << "; b = " << pB;
        linear_approx(&pA,&pD,&pB,1,(globalVal.x->size()-1)/2,globalVal.x->size()-2,
                      globalVal.x,globalVal.y);
    }

    qDebug () << "A = " << pA << "; D = " << pD << "; b = " << pB;

    daX = (xMax-xMin)/5/globalVal.x->size();

    for(int i=0;i<globalVal.x->size()*5;i++){
        aX.append(daX*i);
        aY.append(linear_func(pA,pD,pB,daX*i));
    }
    plot->clearItems();
    QCPItemText *paramText = new QCPItemText(plot);
    paramText->setColor("red");
    paramText->position->setCoords(aX.at((aX.size()-1)/2),aY.at(2));
    paramText->setText("D = "+QString::number(pD)+
                       "\nA = "+QString::number(pA)+
                       "\nb = "+QString::number(pB));
    //plot->addCurve(&medialX,&medialY,true,"red","medial");
    plot->addCurve(&aX,&aY,false,"red","approximate");
}

double windowPlot::linear_func(double A, double D, double B,double x){
    return A*pow(x,-D)+B;
}

void windowPlot::linear_approx(double *A, double *D, double *B, int i1,int i2,int i3,
                               QVector<double> *vX,QVector<double> *vY){
    double pA = *A;
    double pD = *D;
    double pB = *B;

    double eps = 0.001;

    double x1,x2,x3,y1,y2,y3;
    double f1,f2,f3;
    double df11,df12,df13,df21,df22,df23,df31,df32,df33;
    double m11,m12,m13,m21,m22,m23,m31,m32,m33;
    double v1,v2,v3;
    double det,tmp;

    /*
    x1 = globalVal.x->at(i1); y1 = globalVal.y->at(i1);
    x2 = globalVal.x->at(i2); y2 = globalVal.y->at(i2);
    x3 = globalVal.x->at(i3); y3 = globalVal.y->at(i3);
    */

    x1 = vX->at(i1); y1 = vY->at(i1);
    x2 = vX->at(i2); y2 = vY->at(i2);
    x3 = vX->at(i3); y3 = vY->at(i3);


    //значения функции с подставленными значениями
    f1 = linear_func(pA,pD,pB,x1)-y1;
    f2 = linear_func(pA,pD,pB,x2)-y2;
    f3 = linear_func(pA,pD,pB,x3)-y3;

    //qDebug () << "f1: " << f1 << " f2: " << f2 << " f3: " << f3;

    // матрица производных
    df11 = (linear_func(pA+eps,pD,pB,x1)-linear_func(pA,pD,pB,x1))/eps;
    df12 = (linear_func(pA,pD+eps,pB,x1)-linear_func(pA,pD,pB,x1))/eps;
    df13 = (linear_func(pA,pD,pB+eps,x1)-linear_func(pA,pD,pB,x1))/eps;
    df21 = (linear_func(pA+eps,pD,pB,x2)-linear_func(pA,pD,pB,x2))/eps;
    df22 = (linear_func(pA,pD+eps,pB,x2)-linear_func(pA,pD,pB,x2))/eps;
    df23 = (linear_func(pA,pD,pB+eps,x2)-linear_func(pA,pD,pB,x2))/eps;
    df31 = (linear_func(pA+eps,pD,pB,x3)-linear_func(pA,pD,pB,x3))/eps;
    df32 = (linear_func(pA,pD+eps,pB,x3)-linear_func(pA,pD,pB,x3))/eps;
    df33 = (linear_func(pA,pD,pB+eps,x3)-linear_func(pA,pD,pB,x3))/eps;

    // опредилитель матрицы
    det = df11*(df22*df33-df23*df32) +
          df12*(df23*df31-df21*df33) +
          df13*(df21*df32-df22*df31);

    //qDebug () << "det: " << det;
    if(det == 0) return;

    // траспорирование
    tmp = df12; df12 = df21; df21 = tmp;
    tmp = df31; df31 = df13; df13 = tmp;
    tmp = df32; df32 = df23; df23 = tmp;

    // матрица кофакторов
    m11 =  (df22*df33-df23*df32);
    m12 = -(df21*df33-df23*df31);
    m13 =  (df21*df32-df22*df31);

    m21 = -(df12*df33-df13*df32);
    m22 =  (df11*df33-df13*df31);
    m23 = -(df11*df32-df12*df31);

    m31 =  (df12*df23-df13*df22);
    m32 = -(df11*df23-df13*df21);
    m33 =  (df11*df22-df12*df21);

    //обратная матрица
    m11 /= det; m12 /= det; m13 /= det;
    m21 /= det; m22 /= det; m23 /= det;
    m31 /= det; m32 /= det; m33 /= det;

    /*
    qDebug () << m11 << " " <<  m12 << " " << m13;
    qDebug () << m21 << " " <<  m22 << " " << m23;
    qDebug () << m31 << " " <<  m32 << " " << m33;
    */

    v1 = (f1*m11+f2*m12+f3*m13);
    v2 = (f1*m21+f2*m22+f3*m23);
    v3 = (f1*m31+f2*m32+f3*m33);

    //qDebug () << "v1: " << v1 << " v2: " << v2 << " v3: " << v3;
    pA -= v1;
    pD -= v2;
    pB -= v3;

    *A = pA;
    *D = pD;
    *B = pB;
    return;
}
