/*
 * Copyright (C) 2016 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
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

#include "task.h"
#include "csirofacetracker.h"
#include "csvfile.h"
#include "opencv2/opencv.hpp"
#include <QDebug>

using namespace cv;

// +-----------------------------------------------------------
fsdk::Task::Task(QStringList lVideoFiles, QString sCSVFile, QObject *pParent): QObject(pParent)
{
	m_lVideoFiles = lVideoFiles;
	m_sCSVFile = sCSVFile;
	m_bShowProgress = false;
}

// +-----------------------------------------------------------
void fsdk::Task::run()
{
	qDebug() << "Teste de debug";
	qWarning() << "Teste de warning";
	qInfo() << "Teste de info";

	emit finished(0);
}

// +-----------------------------------------------------------
bool fsdk::Task::showProgress() const
{
	return m_bShowProgress;
}

// +-----------------------------------------------------------
void fsdk::Task::setShowProgress(const bool bShow)
{
	m_bShowProgress = bShow;
}