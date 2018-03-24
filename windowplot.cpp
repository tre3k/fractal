#include "windowplot.h"

windowPlot::windowPlot(QWidget *parent) : QMainWindow(parent)
{
    this->setMinimumWidth(600);
    this->setMinimumHeight(350);

    menuBar = new QMenuBar;
    menuFile = new QMenu("&Файл");
    actionSaveTxt = new QAction("Экспорт в txt",this);

    menuScale = new QMenu("Масштаб");
    actionLogX = new QAction("Лог. по х",this);
    actionLogX->setCheckable(true);
    actionLogX->setChecked(true);
    actionLogY = new QAction("Лог. по у",this);
    actionLogY->setCheckable(true);
    actionLogY->setChecked(true);
    actionAutoscale = new QAction("авт.",this);
    actionHideWindow = new QAction("Закрыть", this);

    menuFile->addAction(actionSaveTxt);
    menuFile->addSeparator();
    menuFile->addAction(actionHideWindow);

    menuScale->addAction(actionLogX);
    menuScale->addAction(actionLogY);
    menuScale->addAction(actionAutoscale);

    menuBar->addMenu(menuFile);
    menuBar->addMenu(menuScale);

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

    slot_logX(true);
    slot_logY(true);
    plot->rescaleAxes(true);
    plot->xAxis->scaleRange(1.2);
    plot->yAxis->scaleRange(1.2);
    plot->replot();
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

void windowPlot::slot_close(){
    this->hide();
}
