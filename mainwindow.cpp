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
        data_input->data[i][j] = QString(tmp).toInt();//+(double)(rand()%100)/100;
        //data_input->data[i][j] = cos(2*M_PI*(0.12*i+0.13*j));
        //if(cos(2*M_PI*(0.143*i+0.143*j))>0) data_input->data[i][j] = 1;
        //if(cos(2*M_PI*(0.143*i+0.143*j))<=0) data_input->data[i][j] = 0;
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
        data_fft->data[i][j] = QString(tmp).toInt();
        i++;
        if(i>=data_fft->size_x){
            i=0; j++;
            if(j>=data_fft->size_y) break;
        }
    }

    f.close();
    data_fft_phase = new data2d;
    preProcess();
}

void MainWindow::preProcess(){
    if(data_fft->size_x==0 || data_fft->size_y==0) return;
    plotData(plot_fft,data_fft);
    if(data_fft_phase->size_x!=0 && data_fft_phase->size_y!=0){
        plotData(plot_fft_phase,data_fft_phase);
    }
    plot_fft_phase->plot2D->ColorScale->axis()->setTicker(QSharedPointer<QCPAxisTickerPi>(new QCPAxisTickerPi));
    plot_fft_phase->plot2D->ColorScale->setDataRange(QCPRange(-M_PI/2,M_PI/2));
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

    funcs->average(data_fft,ui->spinBox_center_x->value(),ui->spinBox_center_y->value(),
                 0.0,360.0,ui->spinBox_radius_in->value(),ui->spinBox_radius_our->value(),
                 averX,averY,averErr,ui->checkBox_CKO->isChecked(),0);

    windowPlotValues wPlotValues;

    wPlotValues.err = averErr;
    wPlotValues.x = averX;
    wPlotValues.y = averY;
    wPlotValues.showError = ui->checkBox_Err->isChecked();

    emit signal_plot(wPlotValues);
    winPlot->show();
}

void MainWindow::on_actionScale_triggered()
{
    plot_fft->plot2D->rescaleAxes();
    plot_fft->plot2D->replot();
    plot_input->plot2D->rescaleAxes();
    plot_input->plot2D->replot();
    plot_fft_phase->plot2D->rescaleAxes();
    plot_fft_phase->plot2D->ColorScale->setDataRange(QCPRange(-M_PI/2,M_PI/2));
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

            indata->data[i][sy-j-1] = (double) tmp;
        }
    }

    delete img;

    return;
}

void MainWindow::on_actionOpenImage_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"Image","","All types (*.jpg *.jpeg *.JPG *.JPEG *.bmp *.BMP *.gif "
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
    preProcess();
}

void MainWindow::on_actionOpenImageFFT_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"Image","","All types (*.jpg *.jpeg *.JPG *.JPEG *.bmp *.BMP *.gif "
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
