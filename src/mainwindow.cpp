/*
 *  Copyright (c) 2018-2023 Kirill Pshenichnyi
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

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	this->setWindowTitle(APPLICATION_NAME);
	this->setWindowIcon(QIcon(":/icons/main-icon"));

	/* 80 % height of screen */
	auto WINDOW_SCALE = .8;
	auto window_geometry = (QGuiApplication::primaryScreen())->geometry();
	window_geometry.setHeight(window_geometry.height() * WINDOW_SCALE);
	window_geometry.setWidth(window_geometry.height());

	this->setMinimumSize(100, 100);
	this->setGeometry(window_geometry);

	about_dialog_ = new AboutDialog();

	auto centralLayout = new QGridLayout();
	auto centralWidget = new QWidget();
	centralWidget->setLayout(centralLayout);

	this->setCentralWidget(centralWidget);

	plot_input_ = new iCasePlot2D(tr("Direct space"));
	plot_fft_ = new iCasePlot2D(tr("Fourier space"));
	plot_correlation_ = new iCasePlot2D(tr("Autocorrelation"));
	plot_input_->plot2D->ColorMap->setGradient(
		QCPColorGradient::gpGrayscale
		);
	plot_correlation_->plot2D->ColorMap->setGradient(
		QCPColorGradient::gpGrayscale
		);


	cb_size_of_pixel_ = new QCheckBox();
	cb_size_of_pixel_->setText(tr(
                   "size of object \n"
		   "in direct space \n"
		   "just physical size (m, cm, mm)")
		);
	cb_to_log_ = new QCheckBox();
	cb_to_log_->setText(tr("average to log"));

	dsb_center_x_ = new QDoubleSpinBox();
	dsb_center_y_ = new QDoubleSpinBox();
	dsb_radius_in_ = new QDoubleSpinBox();
	dsb_radius_out_ = new QDoubleSpinBox();

	dsb_center_x_->setDecimals(3);
	dsb_center_x_->setRange(-999999., 999999);
	dsb_center_y_->setDecimals(3);
	dsb_center_y_->setRange(-999999., 999999);
	dsb_radius_in_->setDecimals(3);
	dsb_radius_in_->setRange(0., 999999);
	dsb_radius_out_->setDecimals(3);
	dsb_radius_out_->setRange(0., 999999);

	dsb_open_angle_ = new QDoubleSpinBox();
	dsb_position_angle_ = new QDoubleSpinBox();
	dsb_open_angle_->setRange(0., 360.);
	dsb_open_angle_->setSuffix(tr(" deg."));
	dsb_open_angle_->setValue(10);
	dsb_position_angle_->setRange(-360., 360.);
	dsb_position_angle_->setSuffix(tr(" deg."));

	dsb_size_of_pixel_ = new QDoubleSpinBox();
	dsb_size_of_pixel_->setValue(1.0);
	dsb_size_of_pixel_->setRange(0,9999);
	dsb_size_of_pixel_->setDecimals(3);

	pb_invert_data_ = new QPushButton(tr("inverse data"));
	pb_invert_fft_ = new QPushButton(tr("inverse Fourier"));
	pb_fft_ = new QPushButton(tr("make FFT"));
	pb_center_of_mass_ = new QPushButton(tr("centr. of mass"));
	pb_average_ = new QPushButton(tr("average"));

	win_plot_ = new WindowPlot;
	win_plot_correlation_ = new WindowPlot;

	aver_x_ = new QVector<double>;
	aver_y_ = new QVector<double>;
	aver_err_ = new QVector<double>;

	funcs_ = new Functions;
	fft2d_thread_ = new FFT2DThread;
	connect(fft2d_thread_, &FFT2DThread::complete,
		this, &MainWindow::preProcess);

	correlation_thread_ = new CorrelationThread();
	connect(correlation_thread_, &CorrelationThread::complete,
		this, &MainWindow::completeCorrelation);

	connect(dsb_open_angle_, SIGNAL(valueChanged(double)),
		this, SLOT(changeSpinBox(double)));
	connect(dsb_position_angle_, SIGNAL(valueChanged(double)),
		this, SLOT(changeSpinBox(double)));
	connect(dsb_center_x_, SIGNAL(valueChanged(double)),
		this, SLOT(changeSpinBox(double)));
	connect(dsb_center_y_, SIGNAL(valueChanged(double)),
		this, SLOT(changeSpinBox(double)));
	connect(dsb_radius_in_, SIGNAL(valueChanged(double)),
		this, SLOT(changeSpinBox(double)));
	connect(dsb_radius_out_, SIGNAL(valueChanged(double)),
		this, SLOT(changeSpinBox(double)));
	connect(this, SIGNAL(signal_plot(WindowPlotValues)),
		win_plot_, SLOT(slot_plot(WindowPlotValues)));
	connect(cb_size_of_pixel_, SIGNAL(clicked(bool)),
		this, SLOT(slotChangeRangeFFT()));
	connect(dsb_size_of_pixel_, SIGNAL(valueChanged(double)),
		this, SLOT(slotChangeRangeFFT()));
	connect(cb_size_of_pixel_, SIGNAL(clicked(bool)),
		this, SLOT(gotoCenterMass()));

	connect(pb_invert_data_, &QPushButton::clicked,
		this, &MainWindow::invertData);
	connect(pb_fft_, &QPushButton::clicked,
		this, &MainWindow::buildFFT);
	connect(pb_center_of_mass_, &QPushButton::clicked,
		this, &MainWindow::gotoCenterMass);
	connect(pb_invert_fft_, &QPushButton::clicked,
		this, &MainWindow::invertFFT);
	connect(pb_average_, &QPushButton::clicked,
		this, &MainWindow::Average);

	plot_fft_->checkBoxLog->setChecked(true);
	plot_fft_->slot_log(plot_fft_->checkBoxLog->isChecked());

	auto ltv_controls = new QVBoxLayout();
	auto horizontalLayOutTop = new QHBoxLayout();
	horizontalLayOutTop->addWidget(pb_invert_data_);
	horizontalLayOutTop->addWidget(pb_invert_fft_);
	horizontalLayOutTop->addWidget(pb_fft_);
	horizontalLayOutTop->addWidget(pb_center_of_mass_);

	QGroupBox *gb_average = new QGroupBox(tr("average"));
	auto ltf_sub_average = new QFormLayout();
	gb_average->setLayout(ltf_sub_average);
	ltf_sub_average->addRow(
		tr("sector pos. angle: "),
		dsb_position_angle_
		);
	ltf_sub_average->addRow(
		tr("sector open angle: "),
		dsb_open_angle_
		);
	ltf_sub_average->addRow(
		tr("outer radius: "),
		dsb_radius_out_
		);
	ltf_sub_average->addRow(
		tr("inner radius: "),
		dsb_radius_in_
		);

	auto lth_center_sb = new QHBoxLayout();
	lth_center_sb->addWidget(dsb_center_x_);
	lth_center_sb->addWidget(dsb_center_y_);
	ltf_sub_average->addRow(tr("center: "), lth_center_sb);

	auto lth_average_button = new QHBoxLayout();
	lth_average_button->addWidget(cb_to_log_);
	lth_average_button->addWidget(pb_average_);
	ltf_sub_average->addRow(lth_average_button);

	ltv_controls->addLayout(horizontalLayOutTop);
	ltv_controls->addWidget(gb_average);
	auto lth_size_of_pixel = new QHBoxLayout();
	lth_size_of_pixel->addWidget(dsb_size_of_pixel_);
	lth_size_of_pixel->addWidget(cb_size_of_pixel_);
	ltv_controls->addLayout(lth_size_of_pixel);
	ltv_controls->addSpacing(1);

	centralLayout->addWidget(plot_input_, 0, 0);
	centralLayout->addWidget(plot_fft_, 0, 1);
	centralLayout->addLayout(ltv_controls, 1, 0);
	centralLayout->addWidget(plot_correlation_, 1, 1);

	initActions();
	buildMenuBar();
	buildToolBar();
	buildStatusBar();

	connect(fft2d_thread_, &FFT2DThread::message,
		status_bar_, &QStatusBar::showMessage);
	connect(correlation_thread_, &CorrelationThread::message,
		status_bar_, &QStatusBar::showMessage);

	connect(correlation_thread_, &CorrelationThread::progress,
		sbar_.progress_bar, &QProgressBar::setValue);


	data_input_ = new Data2D;
	data_fft_ = new Data2D;
	data_fft_phase_ = new Data2D;
	data_correlation_ = new Data2D;

}

MainWindow::~MainWindow()
{

}

void MainWindow::initActions() {
	s_actions_.close = new QAction(tr("close"));
	connect(s_actions_.close, &QAction::triggered,
		this, &MainWindow::Close);

	s_actions_.open_text = new QAction(tr("&open (text)"));
	s_actions_.open_text->setIcon(QIcon(":/icons/open-txt"));
	s_actions_.open_text->setIconVisibleInMenu(true);
	connect(s_actions_.open_text, &QAction::triggered,
		this, &MainWindow::slotOpenText);

	s_actions_.open_image = new QAction(tr("open (&image)"));
	s_actions_.open_image->setIcon(QIcon(":/icons/open-img"));
	s_actions_.open_image->setIconVisibleInMenu(true);
	connect(s_actions_.open_image, &QAction::triggered,
		this, &MainWindow::slotOpenImage);

	s_actions_.open_fft = new QAction(tr("open &Fourier (text)"));
	connect(s_actions_.open_fft, &QAction::triggered,
		this, &MainWindow::slotOpenFFT);

	s_actions_.open_image_fft   = new QAction(tr("open Fo&urier (image)"));
	connect(s_actions_.open_image_fft, &QAction::triggered,
		this, &MainWindow::slotOpenImageFFT);

	s_actions_.rescale_axis = new QAction(tr("rescale axis"));
	s_actions_.rescale_axis->setIcon(QIcon(":/icons/rescale"));
	s_actions_.rescale_axis->setIconVisibleInMenu(true);
	connect(s_actions_.rescale_axis, &QAction::triggered,
		this, &MainWindow::slotRescale);

	s_actions_.show_about = new QAction(tr("&about"));
	connect(s_actions_.show_about, &QAction::triggered,
		about_dialog_, &QDialog::show);
}

void MainWindow::buildMenuBar() {
	auto menu_bar = new QMenuBar();
	this->setMenuBar(menu_bar);

	auto m_file = new QMenu(tr("&file"));
	menu_bar->addMenu(m_file);
	m_file->addAction(s_actions_.open_text);
	m_file->addAction(s_actions_.open_image);
	m_file->addAction(s_actions_.open_fft);
	m_file->addAction(s_actions_.open_image_fft);
	m_file->addSeparator();
	m_file->addAction(s_actions_.close);

	auto m_view = new QMenu(tr("&view"));
	menu_bar->addMenu(m_view);
	m_view->addAction(s_actions_.rescale_axis);

	auto m_help = new QMenu(tr("&help"));
	menu_bar->addMenu(m_help);
	m_help->addAction(s_actions_.show_about);
}

void MainWindow::buildToolBar() {
	auto tool_bar = new QToolBar();
	tool_bar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
	tool_bar->setIconSize(QSize(24, 24));
	tool_bar->setMovable(false);

	this->addToolBar(tool_bar);
	tool_bar->addAction(s_actions_.open_text);
	tool_bar->addAction(s_actions_.open_image);
	tool_bar->addSeparator();
	tool_bar->addAction(s_actions_.rescale_axis);
}

void MainWindow::buildStatusBar() {
	status_bar_ = new QStatusBar();
	this->setStatusBar(status_bar_);

	sbar_.progress_bar = new QProgressBar();
	sbar_.progress_bar->setMaximum(100);
	status_bar_->addPermanentWidget(sbar_.progress_bar);
	sbar_.progress_bar->setMaximumSize(100, 13);
}


void MainWindow::plotData(iCasePlot2D *plot, Data2D *dat){
	int i, j;

	plot->plot2D->ColorMap->data()->setSize(dat->size_x, dat->size_y);
	for(i=0; i<dat->size_x; i++){
		for(j=0; j<dat->size_y; j++){
			plot->plot2D->ColorMap->data()->setCell(
				i,
				j,
				dat->data[i][j]
				);
		}
	}
	slotChangeRangeFFT();
	return;
}

void MainWindow::toCircle(double *x, double *y, double r, double phi){
        phi = 2 * M_PI * phi / 360;
        *x = r * cos(phi);
        *y = r * sin(phi);
        return;
}

void MainWindow::paintCircles(iCasePlot2D *plot,
                              double x,
                              double y,
                              double r_in,
                              double r_our,
                              double openAngle,
                              double posAngle){
	plot->plot2D->clearItems();

	QCPItemEllipse *our = new QCPItemEllipse(plot->plot2D);
	our->setPen(QPen(Qt::black));
	our->topLeft->setCoords(x + r_our, y + r_our);
	our->bottomRight->setCoords(x - r_our,y - r_our);

	QCPItemEllipse *in = new QCPItemEllipse(plot->plot2D);
	in->setPen(QPen(Qt::black));
	in->topLeft->setCoords(x + r_in, y + r_in);
	in->bottomRight->setCoords(x - r_in,y - r_in);

	double xl, yl;
	auto line1 = new QCPItemLine(plot->plot2D);
	line1->setPen(QPen(QColor(Qt::black),
			   1,
			   Qt::SolidLine,
			   Qt::SquareCap,
			   Qt::BevelJoin)
		);
	toCircle(&xl, &yl, r_in, posAngle + openAngle / 2);
	line1->start->setCoords(xl + x, yl + y);
	toCircle(&xl, &yl, r_our ,posAngle + openAngle / 2);
	line1->end->setCoords(xl + x, yl + y);

	auto line2 = new QCPItemLine(plot->plot2D);
	line2->setPen(QPen(QColor(Qt::black),
			   1,
			   Qt::SolidLine,
			   Qt::SquareCap,
			   Qt::BevelJoin)
		);
	toCircle(&xl, &yl, r_in, posAngle - openAngle / 2);
	line2->start->setCoords(xl + x, yl + y);
	toCircle(&xl, &yl, r_our, posAngle - openAngle / 2);
	line2->end->setCoords(xl + x, yl + y);

	plot->plot2D->replot();
}

void MainWindow::changeSpinBox(double val){
	paintCircles(plot_fft_,
		     dsb_center_x_->value(),
		     dsb_center_y_->value(),
		     dsb_radius_in_->value(),
		     dsb_radius_out_->value(),
		     dsb_open_angle_->value(),
		     dsb_position_angle_->value());

	dsb_radius_in_->setMaximum(dsb_radius_out_->value());
}

void MainWindow::slotOpenText()
{
	int input_size_x, input_size_y;
	QString tmp;
	QString filename = QFileDialog::getOpenFileName(
		this,
		tr("Open fractal (text format)"),
		"",
		"*.txt"
		);

	if(filename == "") return;
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QTextStream text_stream(&f);

	text_stream >> tmp;
	input_size_x = QString(tmp).toInt();
	text_stream >> tmp;
	input_size_y = QString(tmp).toInt();

        data_input_ = new Data2D(input_size_x, input_size_y);

	int i=0, j=0;

	while(!text_stream.atEnd()){
		text_stream >> tmp;
		data_input_->data[i][j] = QString(tmp).toDouble();
		i++;
		if(i >= data_input_->size_x){
			i=0; j++;
			if(j >= data_input_->size_y) break;
		}
	}

	f.close();

	plotData(plot_input_, data_input_);

	data_fft_ = new Data2D;
	data_fft_phase_ = new Data2D;

	fft2d_thread_->setData(data_input_, data_fft_, data_fft_phase_);
	fft2d_thread_->start();

	buildCorrelation();

	image_loaded_ = true;
}

void MainWindow::slotOpenFFT()
{
	QString tmp;
	int input_size_x,input_size_y;
	QString filename = QFileDialog::getOpenFileName(
		this,
		tr("Open Fourier (image format)"),
		"",
		"*.txt"
		);
	if(filename=="") return;
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QTextStream txtStream(&f);

	txtStream >> tmp;
	input_size_x = QString(tmp).toInt();
	txtStream >> tmp;
	input_size_y = QString(tmp).toInt();

	data_fft_ = new Data2D(input_size_x, input_size_y);

	int i=0, j=0;

	while(!txtStream.atEnd()){
		txtStream >> tmp;
		data_fft_->data[i][j] = QString(tmp).toDouble();
		i++;
		if(i >= data_fft_->size_x){
			i=0; j++;
			if(j >= data_fft_->size_y) break;
		}
	}

	f.close();
	data_fft_phase_ = new Data2D;
	data_input_ = new Data2D;
	preProcess();
}

void MainWindow::preProcess(){
	if(image_loaded_ != true) return;
	if(data_fft_->size_x == 0 || data_fft_->size_y == 0) return;

	status_bar_->showMessage(tr("plotting..."));
	plotData(plot_fft_, data_fft_);

	double c_x = 0, c_y = 0, S = 0;

	Functions::findCenterMass(data_fft_, &c_x, &c_y);

	dsb_center_x_->setValue(c_x);
	dsb_center_y_->setValue(c_y);
	dsb_radius_in_->setValue(0.0);
	double ourRadius_default;


	if(data_fft_->size_x <= data_fft_->size_y){
		ourRadius_default = double(data_fft_->size_x - 1) / 2;
	}else{
		ourRadius_default = double(data_fft_->size_y - 1) / 2;
	}
	dsb_radius_out_->setValue(ourRadius_default);


	if(cb_size_of_pixel_->isChecked()){
		dsb_center_x_->setValue(
			to_impulse_ * (c_x - double(data_fft_->size_x) / 2)
			);
		dsb_center_y_->setValue(
			to_impulse_ * (c_y - double(data_fft_->size_y) / 2)
			);
		dsb_radius_out_->setValue(
			ourRadius_default * to_impulse_ / 2
			);
	}

	status_bar_->showMessage(tr("done."));
}

void MainWindow::Close()
{
	QApplication::exit(0);
}

void MainWindow::Average()
{
	aver_x_->clear();
	aver_y_->clear();
	aver_err_->clear();

	double px_center_x = dsb_center_x_->value();
	double px_center_y = dsb_center_y_->value();
	double px_radius_in = dsb_radius_in_->value();
	double px_radius_out = dsb_radius_out_->value();

	if(cb_size_of_pixel_->isChecked()){
		px_center_x = dsb_center_x_->value() / to_impulse_ +
			double(data_fft_->size_x) / 2;
		px_center_y = dsb_center_y_->value() / to_impulse_ +
			double(data_fft_->size_y) / 2;
		px_radius_in = dsb_radius_in_->value() / to_impulse_;
		px_radius_out = 2*dsb_radius_out_->value() / to_impulse_;
	}

	funcs_->average(data_fft_,
		       px_center_x,
		       px_center_y,
		       dsb_position_angle_->value(),
		       dsb_open_angle_->value(),
		       px_radius_in,px_radius_out,
		       aver_x_,
		       aver_y_,
		       aver_err_,
		       false,
		       0);

	WindowPlotValues wPlotValues;

	wPlotValues.logScale = true;
	if(cb_size_of_pixel_->isChecked()){
		for(int i=0; i < aver_x_->size(); i++){
			(*(aver_x_))[i] = to_impulse_ * aver_x_->at(i) / 2;
		}
	}
	if(cb_to_log_->isChecked()){
		for(int i=0; i < aver_x_->size(); i++){
			if(aver_x_->at(i) == 0){
				aver_x_->remove(i);
				aver_y_->remove(i);
			}
			if(aver_y_->at(i) == 0){
				aver_y_->remove(i);
				aver_y_->remove(i);
			}
		}
		for(int i=0; i < aver_x_->size(); i++){
			(*(aver_x_))[i] = log(aver_x_->at(i));
			(*(aver_y_))[i] = log(aver_y_->at(i));
		}
		wPlotValues.logScale = false;
	}

	wPlotValues.err = aver_err_;
	wPlotValues.x = aver_x_;
	wPlotValues.y = aver_y_;
	wPlotValues.showError = false;

	emit signal_plot(wPlotValues);
	win_plot_->show();

	averageCorrelation();
}

void MainWindow::averageCorrelation() {
	WindowPlotValues win_plot_values;

	emit plotCorrelation(win_plot_values);
}

void MainWindow::slotRescale()
{
	plot_fft_->plot2D->rescaleAxes();
	plot_fft_->plot2D->ColorScale->rescaleDataRange(true);
	plot_fft_->plot2D->replot();

	plot_input_->plot2D->rescaleAxes();
	plot_input_->plot2D->ColorScale->rescaleDataRange(true);
	plot_input_->plot2D->replot();

	plot_correlation_->plot2D->rescaleAxes();
	plot_correlation_->plot2D->ColorScale->rescaleDataRange(true);
	plot_correlation_->plot2D->replot();
}


void MainWindow::openImage(QString filename, Data2D *indata){
	int sx, sy;
	QImage *img = new QImage(filename);
	sx = img->width();
	sy = img->height();

	indata->reinit(sx, sy);
	int tmp;

	for(int i=0; i < sx; i++){
		for(int j=0; j < sy; j++){
			tmp = img->pixel(i, j) & 0x00000000ff;          // B
			tmp += (img->pixel(i, j) & 0x000000ff00)>>8;    // G
			tmp += (img->pixel(i, j) & 0x0000ff0000)>>16;   // R
			tmp /= 3;

			indata->data[i][sy - j - 1] = double(tmp) / 255;
		}
	}

	delete img;
	return;
}

void MainWindow::slotOpenImage()
{
	QString filename = QFileDialog::getOpenFileName(
		this,
		tr("Open fractal (image format)"),
		"",
		"All types (*.jpg *.jpeg *.JPG *.JPEG *.bmp *.BMP *.gif "
		"*.GIF *.png *.PNG *.pbm *.PBM *.pgm *.PGM *.ppm *.PPM "
		"*.xbm *.XBM *.xpm *.XPM);;"
		"Jpeg (*.jpg *.jpeg *.JPG *.JPEG);;"
		"BMP (*.bmp *.BMP);;"
		"GIF (*.gif *.GIF);;"
		"PNG (*.png *.PNG);;"
		"PBM (*.pbm *.PBM);;"
		"PGM (*.pgm *.PGM);;"
		"PPM (*.ppm *.PPM);;"
		"XBM (*.xbm *.XBM);;"
		"XPM (*.xpm *.XPM)"
		);

	if(filename == "") return;
	data_input_ = new Data2D;
	openImage(filename, data_input_);
	plotData(plot_input_, data_input_);


	data_fft_ = new Data2D;
	data_fft_phase_ = new Data2D;

	fft2d_thread_->setData(data_input_, data_fft_, data_fft_phase_);
	fft2d_thread_->start();

	buildCorrelation();

	image_loaded_ = true;
}

void MainWindow::slotOpenImageFFT()
{
	QString filename = QFileDialog::getOpenFileName(
		this,
		"Image",
		"",
		"All types (*.jpg *.jpeg *.JPG *.JPEG *.bmp *.BMP *.gif "
		"*.GIF *.png *.PNG *.pbm *.PBM *.pgm *.PGM *.ppm *.PPM "
		"*.xbm *.XBM *.xpm *.XPM);;"
		"Jpeg (*.jpg *.jpeg *.JPG *.JPEG);;"
		"BMP (*.bmp *.BMP);;"
		"GIF (*.gif *.GIF);;"
		"PNG (*.png *.PNG);;"
		"PBM (*.pbm *.PBM);;"
		"PGM (*.pgm *.PGM);;"
		"PPM (*.ppm *.PPM);;"
		"XBM (*.xbm *.XBM);;"
		"XPM (*.xpm *.XPM)"
		);

	if(filename == "") return;
	data_fft_ = new Data2D;
	openImage(filename, data_fft_);
	plotData(plot_fft_, data_fft_);
	data_fft_phase_ = new Data2D;
	data_input_ = new Data2D;
	image_loaded_ = true;
	preProcess();
}

void MainWindow::invertData()
{
	funcs_->invertData(data_input_);
	plotData(plot_input_, data_input_);
}

void MainWindow::invertFFT()
{
	funcs_->invertData(data_fft_);
	preProcess();
}

void MainWindow::gotoCenterMass()
{
	preProcess();
}

void MainWindow::buildFFT()
{
	data_fft_ = new Data2D;
	data_fft_phase_ = new Data2D;
	fft2d_thread_->setData(data_input_, data_fft_, data_fft_phase_);
	fft2d_thread_->start();
}

void MainWindow::buildCorrelation() {
	if(data_correlation_ != nullptr) data_correlation_ = new Data2D;
	correlation_thread_->setData(data_input_,
				     data_input_,
				     data_correlation_);
	correlation_thread_->start();
	status_bar_->showMessage(tr("calculate correlation..."));
}

void MainWindow::completeCorrelation() {
	plotData(plot_correlation_, data_correlation_);

	slotChangeRangeFFT();
	status_bar_->showMessage(tr("done."));
}

void MainWindow::slotChangeRangeFFT(){
	if((data_fft_ == NULL) ||
	   (data_input_ == NULL) ||
	   (data_fft_phase_ == NULL))
		return;

	if(!cb_size_of_pixel_->isChecked()) {
		plot_fft_->plot2D->ColorMap->data()->setRange(
			QCPRange(0, data_fft_->size_x),
			QCPRange(0, data_fft_->size_y)
			);
		plot_fft_->plot2D->ColorMap->rescaleDataRange(true);
		plot_fft_->plot2D->rescaleAxes();
		plot_fft_->plot2D->replot();

		plot_input_->plot2D->ColorMap->data()->setRange(
			QCPRange(0, data_input_->size_x),
			QCPRange(0, data_input_->size_y));
		plot_input_->plot2D->ColorMap->rescaleDataRange(true);
		plot_input_->plot2D->rescaleAxes();
		plot_input_->plot2D->replot();

		plot_correlation_->plot2D->ColorMap->data()->setRange(
			QCPRange(0, data_correlation_->size_x),
			QCPRange(0, data_correlation_->size_y));
		plot_correlation_->plot2D->ColorMap->rescaleDataRange(true);
		plot_correlation_->plot2D->rescaleAxes();
		plot_correlation_->plot2D->replot();

		to_impulse_ = 1;
		return;
	}

	plot_input_->plot2D->ColorMap->data()->setRange(
		QCPRange(0,dsb_size_of_pixel_->value()),
		QCPRange(0,dsb_size_of_pixel_->value()));
	plot_input_->plot2D->ColorMap->rescaleDataRange(true);
	plot_input_->plot2D->rescaleAxes();
	plot_input_->plot2D->replot();

	plot_correlation_->plot2D->ColorMap->data()->setRange(
		QCPRange(0,dsb_size_of_pixel_->value()),
		QCPRange(0,dsb_size_of_pixel_->value()));
	plot_correlation_->plot2D->ColorMap->rescaleDataRange(true);
	plot_correlation_->plot2D->rescaleAxes();
	plot_correlation_->plot2D->replot();

	plot_fft_->plot2D->ColorMap->data()->setRange(
		QCPRange(-0.5 * 2 * M_PI * data_fft_->size_x /
			 dsb_size_of_pixel_->value(),
			 0.5 * 2 * M_PI * data_fft_->size_x /
			 dsb_size_of_pixel_->value()
			),
		QCPRange(-0.5 * 2 * M_PI * data_fft_->size_y /
			 dsb_size_of_pixel_->value(),
			 0.5 * 2 * M_PI * data_fft_->size_y /
			 dsb_size_of_pixel_->value()
			)
		);

	plot_fft_->plot2D->ColorMap->rescaleDataRange(true);
	plot_fft_->plot2D->rescaleAxes();
	plot_fft_->plot2D->replot();

	to_impulse_ =
		4 * M_PI * data_fft_->size_x /
		dsb_size_of_pixel_->value() /
		data_fft_->size_x;
	return;
}
