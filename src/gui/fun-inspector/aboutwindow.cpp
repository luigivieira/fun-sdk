/*
 * Copyright (C) 2016-2017 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
 *
 * This file is part of Fun SDK (FSDK).
 *
 * FSDK is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FSDK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "aboutwindow.h"
#include "version.h"

#include <QScrollArea>
#include <QLabel>

// +-----------------------------------------------------------
fsdk::AboutWindow::AboutWindow(QWidget *pParent) :
	QDialog(pParent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
	//ui->label->setText(ui->label->text().arg(FSDK_VERSION));
	//ui->label->setOpenExternalLinks(true);
	setFixedSize(425, 268);
}

// +-----------------------------------------------------------
fsdk::AboutWindow::~AboutWindow()
{
    //delete ui;
}
