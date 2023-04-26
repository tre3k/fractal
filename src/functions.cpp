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

#include "functions.h"

Data2D::Data2D(int sx,int sy){
	size_x = sx;
	size_y = sy;
	data = new double * [size_x];
	for(int i=0; i<size_x; i++) data[i] = new double [size_y];
}

void Data2D::remove(){
	for(int i=0; i < size_x; i++) delete [] data[i];
	delete [] *data;
}

void Data2D::reinit(int sx, int sy){
	size_x = sx;
	size_y = sy;
	data = new double * [size_x];
	for(int i=0;i<size_x;i++) data[i] = new double [size_y];
	return;
}

Functions::Functions(){

}

void Functions::fft(double *real, double *imgn, int n){
	int step, start, old_start, i, j, theta=0;
	double tmp_real, tmp_imgn;

	/******** Reverse bits *****/
	int b=0, l, k, bits=log(n)/log(2);
	double *copy_real = new double [n];
	double *copy_imgn = new double [n];
	for(i=0; i < n; i++) copy_real[i] = real[i];
	for(i=0; i < n; i++) copy_imgn[i] = imgn[i];
	for(i=0; i < n; i++){
		b = 0, k = bits;
		for(l = 0; l < bits; l++){
			k--;
			b|=(((i&(1<<l)))>>l)<<k;   // =)
		}

		real[i] = copy_real[b];
		imgn[i] = copy_imgn[b];
	}
	/****************************/

	for(step=1; step<n; step = step << 1){
		old_start = 0;
		for(start = step << 1; start <= n; start += step << 1){
			theta = 0;
			for(i = old_start; i < start - step; i++){
				j = i + step;
				/* a - bc */

				tmp_real = real[i] -
					(real[j] * cos(-theta *
						       2 * M_PI/(step << 1)) -
					 imgn[j] * sin(-theta *
						       2 * M_PI/(step << 1)));
				tmp_imgn = imgn[i] -
					(real[j] * sin(-theta *
						       2 * M_PI/(step << 1)) +
					 imgn[j] * cos(-theta *
						       2 * M_PI/(step << 1)));

				/* a + bc */
				real[i] = real[i] +
					(real[j] * cos(-theta *
						       2 * M_PI/(step << 1)) -
					 imgn[j] * sin(-theta *
						       2 * M_PI/(step << 1)));
				imgn[i] = imgn[i] +
					(real[j] * sin(-theta *
						       2 * M_PI/(step << 1)) +
					 imgn[j] * cos(-theta *
						       2 * M_PI/(step << 1)));

				real[j] = tmp_real;
				imgn[j] = tmp_imgn;

				theta ++;

			}
			old_start = start;
		}
	}
}

void Functions::fft2d(double **real, double **imgn, int N, int M){
	int delta, last_delta, from, to;
	QVector<FFTThread *> fft_threads;
	bool complites {false};

	int input_threads_ = QThread::idealThreadCount();
	auto threads = input_threads_;

	delta = int(M / threads);
	last_delta = M % threads;
	from = 0;
	to = delta;

	if(last_delta != 0) {
		threads ++;
	}else{
		last_delta = delta;
	}

	for(int i = 0; i < threads; i++){
		auto fft_thread = new FFTThread();
		fft_thread->setData(real, imgn, N, M);
		fft_thread->setType(FFTThread::ROWS);
		fft_thread->setFromTo(from, to);
		fft_thread->start();
		fft_threads.append(fft_thread);

		from = to;
		if(i + 2 == threads){
			to = from + last_delta;
		} else {
			to = from + delta;
		}
	}

	while(!complites) {
		complites = fft_threads.at(0)->isComplete();
		for(auto fft_thread : fft_threads) {
			complites &= fft_thread->isComplete();
		}
		QThread::msleep(5);
	}

	for(auto fft_thread : fft_threads) delete fft_thread;
	fft_threads.clear();

	threads = input_threads_;
	delta = int(N / threads);
	last_delta = N % threads;
	from = 0;
	to = delta;

	if(last_delta != 0) {
		threads ++;
	}else{
		last_delta = delta;
	}

	for(int i = 0; i < threads; i++){
		auto fft_thread = new FFTThread();
		fft_thread->setData(real, imgn, N, M);
		fft_thread->setType(FFTThread::COLUMNS);
		fft_thread->setFromTo(from, to);
		fft_thread->start();
		fft_threads.append(fft_thread);

		from = to;
		if(i + 2 == threads){
			to = from + last_delta;
		} else {
			to = from + delta;
		}
	}

	complites = false;
	while(!complites) {
		complites = fft_threads.at(0)->isComplete();
		for(auto fft_thread : fft_threads) {
			complites &= fft_thread->isComplete();
		}
		QThread::msleep(5);
	}

	for(auto fft_thread : fft_threads) delete fft_thread;
	fft_threads.clear();

	sort(real, N, M);
	sort(imgn, N, M);

	return;
}

void Functions::sort(double **mass, int a, int b){
	double tmp = 0.0;
	for(int i=0; i < a; i++){
		for(int j = 0; j < b/2; j++){
			tmp = mass[i][j];
			mass[i][j] = mass[i][b/2 + j];
			mass[i][b/2 + j] = tmp;
		}
	}

	for(int i = 0; i < a/2; i++){
		for(int j = 0; j < b; j++){
			tmp = mass[i][j];
			mass[i][j] = mass[a/2 + i][j];
			mass[a/2 + i][j] = tmp;
		}
	}

	return;
}

void Functions::makeFFT2D(Data2D *data_in,
			  Data2D *data_out,
			  Data2D *data_out_phase){
	double **real;
	double **imgn;
	double dphi = 0.0;
	const int isx = data_in->size_x;
	const int isy = data_in->size_y;

	int nx = 100 * log(isx)/log(2);
	int ny = 100 * log(isy)/log(2);
	int anx,any;

	if(nx%100!=0){
		anx = (int) nx/100+1;
	}else{
		anx = (int) nx/100;
	}
	if(ny%100!=0){
		any = (int) ny/100+1;
	}else{
		any = (int) ny/100;
	}

	int sx = pow(2, anx);
	int sy = pow(2, any);

	if(sx > sy) sy = sx;
	if(sy > sx) sx = sy;

	data_out->reinit(sx, sy);
	data_out_phase->reinit(sx, sy);

	real = new double * [sx];
	imgn = new double * [sx];
	for(int i = 0; i < sx; i++){
		real[i] = new double [sy];
		imgn[i] = new double [sy];
	}

	for(int i = 0; i < sx; i++){
		for(int j = 0; j < sy; j++){
			if(i < isx && j < isy){
				real[i][j] = data_in->data[i][j];
			}else{
				real[i][j] = 0.0;
			}
			imgn[i][j] = 0.0;
		}
	}

	fft2d(real, imgn, sx, sy);

	for(int i=0; i<sx; i++){
		for(int j=0;j<sy;j++){
			data_out->data[i][j] = 4 * (real[i][j] * real[i][j] +
						    imgn[i][j] * imgn[i][j])/
				sx/sx/sy/sy;


			dphi = 0.0;
			if((real[i][j] < 0) && (imgn[i][j] > 0)) dphi = M_PI;
			if((real[i][j] < 0) && (imgn[i][j] < 0)) dphi = -M_PI;

			if(real[i][j] != 0.0){
				data_out_phase->data[i][j] =
					atan(imgn[i][j] / real[i][j]) + dphi;
			}else{
				data_out_phase->data[i][j] = M_PI/2 + dphi;
			}

		}
	}

	for(int i=0; i < sx; i++){
		delete [] real[i];
		delete [] imgn[i];
	}
	delete [] real;
	delete [] imgn;
}


void Functions::makeCorrelationFFT(Data2D *f, Data2D *g, Data2D *out) {

}


int Functions::doubleToInt(double val){
	return (int)(val + 0.5);
}

void Functions::toCircle(double *x, double *y, double r, double phi){
	phi = 2 * M_PI * phi/360;
	*x = r * cos(phi);
	*y = r * sin(phi);
	return;
}

void Functions::average(Data2D *data,
			double x0, double y0,
			double angle, double oangle,
			double offset, double len,
			QVector<double> *vX, QVector<double> *vY,
			QVector<double> *vErr, bool CKO, int step){

	double x,y;
	int oldx=0,oldy=0;
	int ix,iy,n = 0;
	double r = offset,phi;
	double z = 0.0;
	double z2 = 0.0;

	double sigma;

	//double dr = 1;
	double dr = sqrt(2);
	double dphi = 0.1;
	dphi = atan(1.0/len);

	vX->clear();
	vY->clear();
	vErr->clear();

	if(step>0){
		dr = pow(2,step);
		dphi = atan(dr/len);
		if(step>1) dphi = atan(pow(2, step)/len);
	}

	for(r=offset;r<len;r+=dr){
		dphi = atan(1.0/r);
		for(phi=angle-oangle/2;phi<angle+oangle/2;phi+=dphi){
			toCircle(&x,&y,r,phi);

			ix = doubleToInt(x+x0);
			iy = doubleToInt(y+y0);

			if(!(ix==oldx && iy==oldy)){
				if((ix>0 && iy>0) && (ix<data->size_x && iy<data->size_y)){
					z += data->data[ix][iy];
					z2 += data->data[ix][iy]*data->data[ix][iy];
				}else{
					qDebug () << "Out Range!!!";
				}
				n++;

			}
			oldx = ix;
			oldy = iy;
		}

		vX->append(r);
		vY->append((double)z/n);
		if(!CKO){
			vErr->append(sqrt(abs(z/(double)n)));
		}else{
			sigma = sqrt((z*z-z2)/(double)n);
			vErr->append(sqrt(sigma));
		}

		n=0;
		z=0.0;
		z2=0.0;
	}

	return;
}

void Functions::invertData(Data2D *idata){
	if(idata->size_x==0 || idata->size_y==0) return;
	double max=idata->data[0][0];
	for(int i=0;i<idata->size_x;i++){
		for(int j=0;j<idata->size_y;j++){
			if(max<idata->data[i][j]) max = idata->data[i][j];
		}
	}

	for(int i=0;i<idata->size_x;i++){
		for(int j=0;j<idata->size_y;j++){
			idata->data[i][j] = max-idata->data[i][j];
		}
	}
	return;
}


FFTThread::FFTThread() : QThread() {
	is_complete_ = false;
}

volatile bool FFTThread::isComplete() {
	return is_complete_;
}

void FFTThread::setFromTo(int from, int to) {
	from_ = from;
	to_ = to;
}

void FFTThread::setData(double **real, double **imgn, int N, int M) {
	real_ = real;
	imgn_ = imgn;
	N_ = N;
	M_ = M;
}

void FFTThread::setType(Types type){
	type_ = type;
}

void FFTThread::run() {
	is_complete_ = false;
	double *tmp_real;
	double *tmp_imgn;

	switch(type_) {
	case ROWS:
		tmp_real = new double [N_];
		tmp_imgn = new double [N_];

		for(int j = from_; j < to_; j++){

			for(int i=0; i < N_; i++){
				tmp_real[i] = real_[i][j];
				tmp_imgn[i] = imgn_[i][j];
			}

			Functions::fft(tmp_real, tmp_imgn, N_);

			for(int i = 0; i < N_; i++){
				real_[i][j] = tmp_real[i];
				imgn_[i][j] = tmp_imgn[i];
			}
		}

		delete [] tmp_real;
		delete [] tmp_imgn;
		break;

	case COLUMNS:
		tmp_real = new double [M_];
		tmp_imgn = new double [M_];

		for(int i = from_; i < to_; i++){

			for(int j = 0; j < M_; j++){
				tmp_real[j] = real_[i][j];
				tmp_imgn[j] = imgn_[i][j];
			}

			Functions::fft(tmp_real, tmp_imgn, M_);

			for(int j = 0; j < M_; j++){
				real_[i][j] = tmp_real[j];
				imgn_[i][j] = tmp_imgn[j];
			}
		}
		delete [] tmp_real;
		delete [] tmp_imgn;
		break;
	}

	is_complete_ = true;
	emit complete();
}


FFT2DThread::FFT2DThread() : QThread(){

	funcs_ = new Functions;
}

void FFT2DThread::setData(Data2D *data_in,
			  Data2D *data_out,
			  Data2D *data_out_phase) {
	data_in_ = data_in;
	data_out_ = data_out;
	data_out_phase_ = data_out_phase;

	data_is_loaded_ = true;
}

void FFT2DThread::run() {
	if(!data_is_loaded_){
		emit complete();
		return;
	}
	emit message(tr("doing FFT..."), 0);
	funcs_->makeFFT2D(data_in_, data_out_, data_out_phase_);

	data_is_loaded_ = false;
	emit complete();
}


AverageThread::AverageThread() : GeneralThread() {

}

void AverageThread::run() {

}

void AverageThread::setValues(Data2D *data,
			      double x0, double y0,
			      double angle, double oangle,
			      double offset, double len,
			      QVector<double> *vX, QVector<double> *vY,
			      QVector<double> *vErr, bool CKO, int step) {

}

void CorrelationThread::run() {
	int out_size_x {0}, out_size_y {0};

	out_size_x = f_->size_x;
	if(g_->size_x < out_size_x) out_size_x = g_->size_x;
	out_size_y = f_->size_y;
	if(g_->size_y < out_size_y) out_size_y = g_->size_y;

	out_->reinit(out_size_x * 2, out_size_y * 2);

	int chi {0}, yps {0};
	int dchi, dyps;

	double f , g , out;
	unsigned long int counter {0};

	for(chi = -out_size_x; chi < out_size_x; chi ++) {
		for(yps = -out_size_y; yps < out_size_y; yps ++) {
			out = 0.0;
			for(int i = 0; i < out_size_x; i++) {
				for(int j = 0; j < out_size_y; j++) {
					dchi = i - chi;
					dyps = j - yps;
					if((dchi >= 0 && dchi < out_size_x) &&
					   (dyps >= 0 && dyps < out_size_y)) {
						f = f_->data[i][j];
						g = g_->data[dchi][dyps];
						out += f * g;
					}
				}
			}
			out_->data[chi + out_size_x][yps + out_size_y] = out;
			counter ++;
		}

		emit progress((int)(100 * counter/4/out_size_x/out_size_y));
		emit message(tr("calculate correlation..."));
	}

	emit complete();
}

void CorrelationThread::setData(Data2D *f, Data2D *g, Data2D *out) {
	f_ = f;
	g_ = g;
	out_ = out;
}
