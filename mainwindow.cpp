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


#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    plot_input = new iCasePlot2D();
    plot_fft = new iCasePlot2D();
    plot_fft_phase = new iCasePlot2D();
    plot_input->plot2D->ColorMap->setGradient(QCPColorGradient::gpGrayscale);

    ui->layoutPlotIn->addWidget(plot_input);
    ui->layoutPlotFFT->addWidget(plot_fft);
    ui->layoutPlotFFT_phase->addWidget(plot_fft_phase);

    winPlot = new windowPlot;

    averX = new QVector<double>;
    averY = new QVector<double>;
    averErr = new QVector<double>;

    funcs = new functions;

    connect(ui->spinBox_center_x,SIGNAL(valueChanged(double)),
            this,SLOT(slot_changeSpinBoxs(double)));
    connect(ui->spinBox_center_y,SIGNAL(valueChanged(double)),
            this,SLOT(slot_changeSpinBoxs(double)));
    connect(ui->spinBox_radius_in,SIGNAL(valueChanged(double)),
            this,SLOT(slot_changeSpinBoxs(double)));
    connect(ui->spinBox_radius_our,SIGNAL(valueChanged(double)),
            this,SLOT(slot_changeSpinBoxs(double)));
    connect(this,SIGNAL(signal_plot(windowPlotValues)),
            winPlot,SLOT(slot_plot(windowPlotValues)));
    connect(ui->checkBoxSizeOfPixel,SIGNAL(clicked(bool)),
            this,SLOT(slotChangeRangeFFT()));
    connect(ui->SpinBoxSizeOfPixel,SIGNAL(valueChanged(double)),
            this,SLOT(slotChangeRangeFFT()));
    connect(ui->checkBoxSizeOfPixel,SIGNAL(clicked(bool)),
            this,SLOT(on_pushButtonCentre_clicked()));

    plot_fft->checkBoxLog->setChecked(true);
    plot_fft->slot_log(plot_fft->checkBoxLog->isChecked());


    QWidget *centralWidget = new QWidget();
    QGridLayout *centralLayout = new QGridLayout();

    centralWidget->setLayout(centralLayout);
    this->setCentralWidget(centralWidget);

    QVBoxLayout *subLayout = new QVBoxLayout;
    QHBoxLayout *horizontalLayOutTop = new QHBoxLayout;
    horizontalLayOutTop->addWidget(ui->pushButton_invertData);
    horizontalLayOutTop->addWidget(ui->pushButton_invertFFT);
    horizontalLayOutTop->addWidget(ui->pushButton_FFT);
    horizontalLayOutTop->addWidget(ui->pushButtonCentre);
    QGroupBox *groupAverage = new QGroupBox("average");

    QGridLayout *subAverageLayout = new QGridLayout;
    groupAverage->setLayout(subAverageLayout);
    subAverageLayout->addWidget(ui->label_4,0,0);
    subAverageLayout->addWidget(ui->label_2,1,0);
    subAverageLayout->addWidget(ui->label_3,2,0);
    subAverageLayout->addWidget(ui->label,3,0);

    subAverageLayout->addWidget(ui->spinBox_openAngle,0,1);
    subAverageLayout->addWidget(ui->spinBox_radius_our,1,1);
    subAverageLayout->addWidget(ui->spinBox_radius_in,2,1);
    QHBoxLayout *centerLayout = new QHBoxLayout;
    centerLayout->addWidget(ui->spinBox_center_x);
    centerLayout->addWidget(ui->spinBox_center_y);
    subAverageLayout->addLayout(centerLayout,3,1);

    subAverageLayout->addWidget(ui->checkBoxLog,2,2);
    subAverageLayout->addWidget(ui->pushButtonIntegrate,3,2);

    QHBoxLayout *horizontalLayOutBottom = new QHBoxLayout;
    horizontalLayOutBottom->addWidget(ui->SpinBoxSizeOfPixel);
    horizontalLayOutBottom->addWidget(ui->checkBoxSizeOfPixel);

    subLayout->addLayout(horizontalLayOutTop);
    subLayout->addWidget(groupAverage);
    subLayout->addLayout(horizontalLayOutBottom);
    subLayout->addSpacing(100);

    centralLayout->addWidget(plot_input,0,0);
    centralLayout->addWidget(plot_fft,0,1);
    centralLayout->addLayout(subLayout,1,0);
    centralLayout->addWidget(plot_fft_phase,1,1);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::plotData(iCasePlot2D *plot, data2d *dat){
    int i,j;

    //plot->plot2D->ColorMap->data()->setRange(QCPRange(0,dat->size_x),QCPRange(0,dat->size_y));
    plot->plot2D->ColorMap->data()->setSize(dat->size_x,dat->size_y);
    for(i=0;i<dat->size_x;i++){
        for(j=0;j<dat->size_y;j++){
            plot->plot2D->ColorMap->data()->setCell(i,j,dat->data[i][j]);
        }
    }
    slotChangeRangeFFT();
    /*
    plot->plot2D->ColorMap->rescaleDataRange(true);
    plot->plot2D->rescaleAxes();
    plot->plot2D->replot();
    */
    return;
}

void MainWindow::paintCircles(iCasePlot2D *plot,double x, double y, double r_in, double r_our){
    if(!ui->checkBoxSizeOfPixel->isChecked()){
        x+=0.5;
        y+=0.5;
    }
    plot->plot2D->clearItems();
    QCPItemEllipse *our = new QCPItemEllipse(plot->plot2D);
    //our->setPen(QPen(Qt::white));
    our->setPen(QPen(Qt::black));
    our->topLeft->setCoords(x+r_our,y+r_our);
    our->bottomRight->setCoords(x-r_our,y-r_our);
    QCPItemEllipse *in = new QCPItemEllipse(plot->plot2D);
    //in->setPen(QPen(Qt::white));
    in->setPen(QPen(Qt::black));
    in->topLeft->setCoords(x+r_in,y+r_in);
    in->bottomRight->setCoords(x-r_in,y-r_in);
    plot->plot2D->replot();
}

void MainWindow::slot_changeSpinBoxs(double val){
    paintCircles(plot_fft,
                 ui->spinBox_center_x->value(),
                 ui->spinBox_center_y->value(),
                 ui->spinBox_radius_in->value(),
                 ui->spinBox_radius_our->value());
    ui->spinBox_radius_in->setMaximum(ui->spinBox_radius_our->value());
}

void MainWindow::on_action_Open_triggered()
{
    int input_size_x,input_size_y;
    QString tmp;
    QString filename = QFileDialog::getOpenFileName(this,"Fractal","","*.txt");
    if(filename=="") return;
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QTextStream txtStream(&f);


    txtStream >> tmp;
    input_size_x = QString(tmp).toInt();
    txtStream >> tmp;
    input_size_y = QString(tmp).toInt();

    data_input = new data2d(input_size_x,input_size_y);

    int i=0,j=0;

    while(!txtStream.atEnd()){
        txtStream >> tmp;
        data_input->data[i][j] = QString(tmp).toDouble();//+(double)(rand()%100)/100;
        i++;
        if(i>=data_input->size_x){
            i=0; j++;
            if(j>=data_input->size_y) break;
        }
    }

    f.close();

    plotData(plot_input,data_input);

    data_fft = new data2d;
    data_fft_phase = new data2d;
    funcs->makeFFT2D(data_input,data_fft,data_fft_phase);
    imageLoaded=true;
    preProcess();
}

void MainWindow::on_action_openFFT_triggered()
{
    QString tmp;
    int input_size_x,input_size_y;
    QString filename = QFileDialog::getOpenFileName(this,"Fractal","","*.txt");
    if(filename=="") return;
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QTextStream txtStream(&f);

    txtStream >> tmp;
    input_size_x = QString(tmp).toInt();
    txtStream >> tmp;
    input_size_y = QString(tmp).toInt();

    data_fft = new data2d(input_size_x,input_size_y);

    int i=0,j=0;

    while(!txtStream.atEnd()){
        txtStream >> tmp;
        data_fft->data[i][j] = QString(tmp).toDouble();
        i++;
        if(i>=data_fft->size_x){
            i=0; j++;
            if(j>=data_fft->size_y) break;
        }
    }

    f.close();
    data_fft_phase = new data2d;
    data_input = new data2d;
    preProcess();
}

void MainWindow::preProcess(){
    if(imageLoaded!=true) return;
    if(data_fft->size_x==0 || data_fft->size_y==0) return;
    plotData(plot_fft,data_fft);
    if(data_fft_phase->size_x!=0 && data_fft_phase->size_y!=0){
        plotData(plot_fft_phase,data_fft_phase);
    }
    plot_fft_phase->plot2D->ColorScale->axis()->
    setTicker(QSharedPointer<QCPAxisTickerPi>(new QCPAxisTickerPi));
    plot_fft_phase->plot2D->ColorScale->setDataRange(QCPRange(-M_PI,M_PI));
    plot_fft_phase->plot2D->replot();

    double c_x=0,c_y=0,S=0;

    for(int i=0;i<data_fft->size_x;i++){
        for(int j=0;j<data_fft->size_y;j++){
            c_x += data_fft->data[i][j]*i;
            c_y += data_fft->data[i][j]*j;
            S += data_fft->data[i][j];
        }
    }
    c_x /= S;
    c_y /= S;

    ui->spinBox_center_x->setValue(c_x);
    ui->spinBox_center_y->setValue(c_y);
    ui->spinBox_radius_in->setValue(0.0);
    double ourRadius_default;


    if(data_fft->size_x<=data_fft->size_y){
        ourRadius_default=(double)(data_fft->size_x-1)/2;
    }else{
        ourRadius_default=(double)(data_fft->size_y-1)/2;
    }
    ui->spinBox_radius_our->setValue(ourRadius_default);


    if(ui->checkBoxSizeOfPixel->isChecked()){
        ui->spinBox_center_x->setValue(toImpulse*(c_x-data_fft->size_x/2));
        ui->spinBox_center_y->setValue(toImpulse*(c_y-data_fft->size_y/2));
        ui->spinBox_radius_our->setValue(ourRadius_default*toImpulse/2);
    }

}

void MainWindow::on_action_Close_triggered()
{
    QApplication::exit(0);
}

void MainWindow::on_pushButtonIntegrate_clicked()
{
    averX->clear();
    averY->clear();
    averErr->clear();

    double px_center_x = ui->spinBox_center_x->value();
    double px_center_y = ui->spinBox_center_y->value();
    double px_radius_in = ui->spinBox_radius_in->value();
    double px_radius_our = ui->spinBox_radius_our->value();

    if(ui->checkBoxSizeOfPixel->isChecked()){
        px_center_x = ui->spinBox_center_x->value()/toImpulse + data_fft->size_x/2;
        px_center_y = ui->spinBox_center_y->value()/toImpulse + data_fft->size_y/2;
        px_radius_in = ui->spinBox_radius_in->value()/toImpulse;
        px_radius_our = 2*ui->spinBox_radius_our->value()/toImpulse;
    }


    funcs->average(data_fft,px_center_x,px_center_y,
                 0.0, ui->spinBox_openAngle->value(), px_radius_in,px_radius_our,
                 averX,averY,averErr,false,0);

    windowPlotValues wPlotValues;

    wPlotValues.logScale = true;
    if(ui->checkBoxSizeOfPixel->isChecked()){
        for(int i=0;i<averX->size();i++){
            (*(averX))[i] = toImpulse*averX->at(i)/2;
        }
    }
    if(ui->checkBoxLog->isChecked()){
        for(int i=0;i<averX->size();i++){
            if(averX->at(i)==0) averX->remove(i);
            if(averY->at(i)==0) averY->remove(i);
        }
        for(int i=0;i<averX->size();i++){
            (*(averX))[i] = log(averX->at(i));
            (*(averY))[i] = log(averY->at(i));
        }
        wPlotValues.logScale = false;
    }

    wPlotValues.err = averErr;
    wPlotValues.x = averX;
    wPlotValues.y = averY;
    wPlotValues.showError = false;

    emit signal_plot(wPlotValues);
    winPlot->show();
}

void MainWindow::on_actionScale_triggered()
{
    plot_fft->plot2D->rescaleAxes();
    plot_fft->plot2D->ColorScale->rescaleDataRange(true);
    plot_fft->plot2D->replot();

    plot_input->plot2D->rescaleAxes();
    plot_input->plot2D->ColorScale->rescaleDataRange(true);
    plot_input->plot2D->replot();

    plot_fft_phase->plot2D->rescaleAxes();
    plot_fft_phase->plot2D->ColorScale->setDataRange(QCPRange(-M_PI,M_PI));
    plot_fft_phase->plot2D->replot();
}


void MainWindow::openImage(QString filename,data2d *indata){
    int sx,sy;
    QImage *img = new QImage(filename);
    sx = img->width();
    sy = img->height();

    indata->reinit(sx,sy);
    int tmp;

    for(int i=0;i<sx;i++){
        for(int j=0;j<sy;j++){

            tmp = img->pixel(i,j) & 0x00000000ff;                    //B
            tmp += (img->pixel(i,j) & 0x000000ff00)>>8;              //G
            tmp += (img->pixel(i,j) & 0x0000ff0000)>>16;             //R
            tmp /= 3;

            indata->data[i][sy-j-1] = (double)(tmp)/255;
        }
    }

    delete img;

    return;
}

void MainWindow::on_actionOpenImage_triggered()
{
    QString filename = QFileDialog::getOpenFileName
    (this,"Image","","All types (*.jpg *.jpeg *.JPG *.JPEG *.bmp *.BMP *.gif "
    "*.GIF *.png *.PNG *.pbm *.PBM *.pgm *.PGM *.ppm *.PPM *.xbm *.XBM *.xpm *.XPM);;"
    "Jpeg (*.jpg *.jpeg *.JPG *.JPEG);;"
    "BMP (*.bmp *.BMP);;"
    "GIF (*.gif *.GIF);;"
    "PNG (*.png *.PNG);;"
    "PBM (*.pbm *.PBM);;"
    "PGM (*.pgm *.PGM);;"
    "PPM (*.ppm *.PPM);;"
    "XBM (*.xbm *.XBM);;"
    "XPM (*.xpm *.XPM)");

    if(filename == "") return;
    data_input = new data2d;
    openImage(filename,data_input);
    plotData(plot_input,data_input);


    data_fft = new data2d;
    data_fft_phase = new data2d;
    funcs->makeFFT2D(data_input,data_fft,data_fft_phase);
    imageLoaded=true;
    preProcess();
}

void MainWindow::on_actionOpenImageFFT_triggered()
{
    QString filename = QFileDialog::getOpenFileName(
    this,"Image","","All types (*.jpg *.jpeg *.JPG *.JPEG *.bmp *.BMP *.gif "
    "*.GIF *.png *.PNG *.pbm *.PBM *.pgm *.PGM *.ppm *.PPM *.xbm *.XBM *.xpm *.XPM);;"
    "Jpeg (*.jpg *.jpeg *.JPG *.JPEG);;"
    "BMP (*.bmp *.BMP);;"
    "GIF (*.gif *.GIF);;"
    "PNG (*.png *.PNG);;"
    "PBM (*.pbm *.PBM);;"
    "PGM (*.pgm *.PGM);;"
    "PPM (*.ppm *.PPM);;"
    "XBM (*.xbm *.XBM);;"
    "XPM (*.xpm *.XPM)");

    if(filename == "") return;
    data_fft = new data2d;
    openImage(filename,data_fft);
    plotData(plot_fft,data_fft);
    data_fft_phase = new data2d;
    data_input = new data2d;
    preProcess();
}

void MainWindow::on_pushButton_invertData_clicked()
{
    funcs->inverteData(data_input);
    plotData(plot_input,data_input);
}

void MainWindow::on_pushButton_invertFFT_clicked()
{
    funcs->inverteData(data_fft);
    //funcs->inverteData(data_fft_phase);
    preProcess();
}
    
void MainWindow::on_pushButtonCentre_clicked()
{
    preProcess();
}

void MainWindow::on_pushButton_FFT_clicked()
{
    data_fft = new data2d;
    data_fft_phase = new data2d;
    funcs->makeFFT2D(data_input,data_fft,data_fft_phase);
    preProcess();
}

void MainWindow::slotChangeRangeFFT(){
    if((data_fft == NULL) || (data_input == NULL) || (data_fft_phase == NULL)) return;
    if(!ui->checkBoxSizeOfPixel->isChecked()){
        plot_fft->plot2D->ColorMap->data()->setRange(QCPRange(0,data_fft->size_x),QCPRange(0,data_fft->size_y));
        plot_fft->plot2D->ColorMap->rescaleDataRange(true);
        plot_fft->plot2D->rescaleAxes();
        plot_fft->plot2D->replot();

        plot_fft_phase->plot2D->ColorMap->data()->setRange(QCPRange(0,data_fft->size_x),QCPRange(0,data_fft->size_y));
        plot_fft_phase->plot2D->ColorMap->rescaleDataRange(true);
        plot_fft_phase->plot2D->rescaleAxes();
        plot_fft_phase->plot2D->replot();

        plot_input->plot2D->ColorMap->data()->setRange(QCPRange(0,data_input->size_x),QCPRange(0,data_input->size_y));
        plot_input->plot2D->ColorMap->rescaleDataRange(true);
        plot_input->plot2D->rescaleAxes();
        plot_input->plot2D->replot();
        toImpulse = 1;
        return;
    }

    /*
    plot_input->plot2D->ColorMap->data()->setRange(QCPRange(0,data_input->size_x*ui->SpinBoxSizeOfPixel->value()),
                                                   QCPRange(0,data_input->size_y*ui->SpinBoxSizeOfPixel->value()));
    */
    plot_input->plot2D->ColorMap->data()->setRange(QCPRange(0,ui->SpinBoxSizeOfPixel->value()),
                                                   QCPRange(0,ui->SpinBoxSizeOfPixel->value()));
    plot_input->plot2D->ColorMap->rescaleDataRange(true);
    plot_input->plot2D->rescaleAxes();
    plot_input->plot2D->replot();

    /*
    plot_fft->plot2D->ColorMap->data()->setRange(QCPRange(-0.5*2*M_PI/ui->SpinBoxSizeOfPixel->value(),
                                                          0.5*2*M_PI/ui->SpinBoxSizeOfPixel->value()),
                                                 QCPRange(-0.5*2*M_PI/ui->SpinBoxSizeOfPixel->value(),
                                                          0.5*2*M_PI/ui->SpinBoxSizeOfPixel->value()));
     */

    plot_fft->plot2D->ColorMap->data()->setRange(QCPRange(-0.5*2*M_PI*data_fft->size_x/ui->SpinBoxSizeOfPixel->value(),
                                                          0.5*2*M_PI*data_fft->size_x/ui->SpinBoxSizeOfPixel->value()),
                                                 QCPRange(-0.5*2*M_PI*data_fft->size_y/ui->SpinBoxSizeOfPixel->value(),
                                                          0.5*2*M_PI*data_fft->size_y/ui->SpinBoxSizeOfPixel->value()));

    plot_fft->plot2D->ColorMap->rescaleDataRange(true);
    plot_fft->plot2D->rescaleAxes();
    plot_fft->plot2D->replot();
    /*
    plot_fft_phase->plot2D->ColorMap->data()->setRange(QCPRange(-0.5*2*M_PI/ui->SpinBoxSizeOfPixel->value(),
                                                          0.5*2*M_PI/ui->SpinBoxSizeOfPixel->value()),
                                                 QCPRange(-0.5*2*M_PI/ui->SpinBoxSizeOfPixel->value(),
                                                          0.5*2*M_PI/ui->SpinBoxSizeOfPixel->value()));
                                                          */
    plot_fft_phase->plot2D->ColorMap->data()->setRange(QCPRange(-0.5*2*M_PI*data_fft_phase->size_x/ui->SpinBoxSizeOfPixel->value(),
                                                          0.5*2*M_PI*data_fft_phase->size_x/ui->SpinBoxSizeOfPixel->value()),
                                                 QCPRange(-0.5*2*M_PI*data_fft_phase->size_y/ui->SpinBoxSizeOfPixel->value(),
                                                          0.5*2*M_PI*data_fft_phase->size_y/ui->SpinBoxSizeOfPixel->value()));

    plot_fft_phase->plot2D->ColorMap->rescaleDataRange(true);
    plot_fft_phase->plot2D->rescaleAxes();
    plot_fft_phase->plot2D->replot();
    //toImpulse = 4*M_PI/ui->SpinBoxSizeOfPixel->value()/data_fft->size_x;
    toImpulse = 4*M_PI*data_fft->size_x/ui->SpinBoxSizeOfPixel->value()/data_fft->size_x;
    return;
}
