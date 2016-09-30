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
	VideoCapture oCap;
	Mat oFrame;

	for(int i = 0; i < m_lVideoFiles.count(); i++)
	{
		QString sFile = m_lVideoFiles[i];
		qInfo(qPrintable(tr("processing video %d/%d: %s")), i+1, m_lVideoFiles.count(), qPrintable(sFile));
		
		if(!oCap.open(sFile.toStdString()))
		{
			qCritical(qPrintable(tr("could not read file %s")), qPrintable(sFile));
			emit finished(i + 2);
			return;
		}

		int iFrameCount = oCap.get(CV_CAP_PROP_FRAME_COUNT);
		int iProg = 0, iLastProg = -1;
		qDebug("\t");
		qDebug(qPrintable(tr("completed:")));
		qDebug("     ");
		for(int iFrame = 0; iFrame < iFrameCount; iFrame++)
		{
			iProg = qRound(float(iFrame) / float(iFrameCount) * 100.0f);
			if(iProg != iLastProg)
			{
				qDebug("\b\b\b\b%3d%%", iProg);
				iLastProg = iProg;
			}
			oCap >> oFrame;
		}
		qDebug() << endl;

		oCap.release();
	}

	qInfo(qPrintable(tr("saving data to %s")), qPrintable(m_sCSVFile));

	qInfo(qPrintable(tr("done.")));
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