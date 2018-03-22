#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    plot_input = new iCasePlot2D();
    plot_fft = new iCasePlot2D();
    plot_input->plot2D->ColorMap->setGradient(QCPColorGradient::gpGrayscale);

    ui->layoutPlotIn->addWidget(plot_input);
    ui->layoutPlotFFT->addWidget(plot_fft);

    winPlot = new windowPlot;

    averX = new QVector<double>;
    averY = new QVector<double>;
    averErr = new QVector<double>;

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::plotData(iCasePlot2D *plot, data2d *dat){
    int i,j;
    plot->plot2D->ColorMap->data()->setRange(QCPRange(0,dat->size_x),QCPRange(0,dat->size_y));
    plot->plot2D->ColorMap->data()->setSize(dat->size_x,dat->size_y);
    for(i=0;i<dat->size_x;i++){
        for(j=0;j<dat->size_y;j++){
            plot->plot2D->ColorMap->data()->setCell(i,j,dat->data[i][j]);
        }
    }
    plot->plot2D->ColorMap->rescaleDataRange(true);
    plot->plot2D->rescaleAxes();
    plot->plot2D->replot();
    return;
}

void MainWindow::paintCircles(iCasePlot2D *plot,double x, double y, double r_in, double r_our){
    plot->plot2D->clearItems();
    QCPItemEllipse *our = new QCPItemEllipse(plot->plot2D);
    our->setPen(QPen(Qt::white));
    our->topLeft->setCoords(x+r_our,y+r_our);
    our->bottomRight->setCoords(x-r_our,y-r_our);
    QCPItemEllipse *in = new QCPItemEllipse(plot->plot2D);
    in->setPen(QPen(Qt::white));
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
    int fft_size_x,fft_size_y;
    int n2;
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
    n2 = (int)(log(input_size_x)/log(2));
    fft_size_x = pow(2,n2);
    n2 = (int)(log(input_size_y)/log(2));
    fft_size_y = pow(2,n2);
    data_fft = new data2d(fft_size_x,fft_size_y);

    int i=0,j=0;

    while(!txtStream.atEnd()){
        txtStream >> tmp;
        data_input->data[i][j] = QString(tmp).toInt();
        i++;
        if(i>=data_input->size_x){
            i=0; j++;
            if(j>=data_input->size_y) break;
        }
    }

    f.close();

    plotData(plot_input,data_input);

    functions *funcs = new functions;
    funcs->makeFFT2D(data_input,data_fft);
    delete funcs;
    preProcess();
}

void MainWindow::preProcess(){
    plotData(plot_fft,data_fft);

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
}

void MainWindow::on_action_Close_triggered()
{
    data_input->remove();
    delete data_input;
    QApplication::exit(0);
}



void MainWindow::on_pushButtonIntegrate_clicked()
{
    averX->clear();
    averY->clear();
    averErr->clear();

    functions *funcs = new functions;
    funcs->average(data_fft,ui->spinBox_center_x->value(),ui->spinBox_center_y->value(),
                 0.0,360.0,ui->spinBox_radius_in->value(),ui->spinBox_radius_our->value(),
                 averX,averY,averErr,ui->checkBox_CKO->isChecked(),0);
    delete funcs;

    windowPlotValues wPlotValues;

    wPlotValues.err = averErr;
    wPlotValues.x = averX;
    wPlotValues.y = averY;

    emit signal_plot(wPlotValues);
    winPlot->show();
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
        data_fft->data[i][j] = QString(tmp).toInt();
        i++;
        if(i>=data_fft->size_x){
            i=0; j++;
            if(j>=data_fft->size_y) break;
        }
    }

    f.close();
    preProcess();
}

void MainWindow::on_actionScale_triggered()
{
    plot_fft->plot2D->rescaleAxes();
    plot_fft->plot2D->replot();
    plot_input->plot2D->rescaleAxes();
    plot_input->plot2D->replot();
}

void MainWindow::on_actionOpenMatrix_triggered()
{
    QString tmp;
    int input_size_x,input_size_y;
    QString filename = QFileDialog::getOpenFileName(this,"Matrix","","*.txt");
    if(filename=="") return;
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QTextStream txtStream(&f);


    int i=0;
    while(!txtStream.atEnd()){
        txtStream >> tmp;
        qDebug() << i << tmp;
        i++;
    }


}
