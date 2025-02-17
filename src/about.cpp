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


#include "about.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
	this->setWindowTitle("About");

	this->setWindowIcon(QIcon(":/icons/main-icon"));

	auto btn_close = new QPushButton("close");
	auto ltv_main = new QVBoxLayout();
	auto lth_buttons = new QHBoxLayout();
	auto lb_pixmap = new QLabel();
	QPixmap logo(":/icons/main-icon");
	lb_pixmap->setPixmap(
		logo.scaled(
			70,
			70,
			Qt::KeepAspectRatio,
			Qt::SmoothTransformation)
		);

	auto label = new QLabel(
		"<b>" + QString(APPLICATION_NAME) + " " +
		QString(APPLICATION_VERSION) + "</b><br><hr>" +
		tr("Under license") +  " GNU GLPv3 © " +
		QString(COPYRIGHT_YEAR) + "<br>" +
		tr("Autor: Kirill Pshenichnyi ") +
		"&lt;pshcyrill@mail.ru&gt;<br>" +
		APPLICATION_DESCRIPTION +
		"<hr>" +
		"Built: " + QString(BUILT_COMPILER_ID) +
		" " + QString(BUILT_COMPILER_VERSION) +
		+ " compiler"+
		" (" + QString(BUILT_TIMESTAMP) + ")"
		);

	ltv_main->addWidget(lb_pixmap);
	ltv_main->addWidget(label);

	connect(btn_close, &QPushButton::released,
		this, &QDialog::hide);

	lth_buttons->addStretch();
	lth_buttons->addWidget(btn_close);
	ltv_main->addLayout(lth_buttons);
	this->setLayout(ltv_main);
}
