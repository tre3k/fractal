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


#include "about.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
	this->setWindowTitle("About");

	this->setWindowIcon(QIcon(":/icons/main-icon"));

	auto buttonClose = new QPushButton("close");
	auto layoutMain = new QVBoxLayout();
	auto layoutButtons = new QHBoxLayout();
	auto labelPixmap = new QLabel();
	QPixmap logo(":/icons/main-icon");
	labelPixmap->setPixmap(logo.scaled(70,
					   70,
					   Qt::KeepAspectRatio,
					   Qt::SmoothTransformation));

	auto label = new QLabel("<b>" + QString(APPLICATION_NAME) + " " +
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

	layoutMain->addWidget(labelPixmap);
	layoutMain->addWidget(label);

	connect(buttonClose, &QPushButton::released,
		this, &QDialog::hide);

	layoutButtons->addStretch();
	layoutButtons->addWidget(buttonClose);
	layoutMain->addLayout(layoutButtons);
	this->setLayout(layoutMain);

}
