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

#include "consoleapp.h"
#include <QDebug>
#include <QDir>
#include <QThreadPool>
#include "featureextractor.h"

// +-----------------------------------------------------------
fsdk::ConsoleApp::ConsoleApp(QStringList lVideoFiles, QString sOutputDir, QObject *pParent): QObject(pParent)
{
	m_lVideoFiles = lVideoFiles;
	m_sOutputDir = sOutputDir;
}

// +-----------------------------------------------------------
void fsdk::ConsoleApp::run()
{
	foreach(QString sFile, m_lVideoFiles)
	{
		QFileInfo oFile(sFile);
		QString sLandmarks = QString("%1/%2-landmarks.csv").arg(m_sOutputDir, oFile.baseName());
		FeatureExtractor *pTask = new FeatureExtractor(sFile, sLandmarks);
		pTask->setAutoDelete(false);
		m_lTasks.append(pTask);

		connect(pTask, &FeatureExtractor::error, this, &ConsoleApp::onTaskError);
		connect(pTask, &FeatureExtractor::progress, this, &ConsoleApp::onTaskProgress);
		connect(pTask, &FeatureExtractor::finished, this, &ConsoleApp::onTaskFinished);
		
		QThreadPool::globalInstance()->start(pTask);
	}
}

// +-----------------------------------------------------------
void fsdk::ConsoleApp::onTaskError(QString sVideoFile, FeatureExtractor::ExtractionError eError)
{
	qCritical(qPrintable(tr("error on file %1").arg(sVideoFile)));

	// Delete the pointer to the task that generated error
	FeatureExtractor *pErrorTask = static_cast<FeatureExtractor*>(sender());
	m_lTasks.removeOne(pErrorTask);
	delete pErrorTask;

	// Signal the cancellation to all remainint tasks
	foreach(FeatureExtractor *pTask, m_lTasks)
		pTask->cancel();
	QThreadPool::globalInstance()->waitForDone();

	// Delete the remaining pointers
	foreach(FeatureExtractor *pTask, m_lTasks)
		delete pTask;
	m_lTasks.clear();
	
	// Signal the termination with the error code as the index of the task
	int iError = m_lVideoFiles.indexOf(sVideoFile) + 1;
	m_lVideoFiles.clear();
	emit finished(iError);
}

// +-----------------------------------------------------------
void fsdk::ConsoleApp::onTaskProgress(QString sVideoFile, int iProgress)
{
	qDebug(qPrintable(tr("file %1 progress: %2%%").arg(sVideoFile).arg(iProgress)));
}

// +-----------------------------------------------------------
void fsdk::ConsoleApp::onTaskFinished(QString sVideoFile)
{
	qInfo(qPrintable(tr("file %1 done.").arg(sVideoFile)));

	// Delete the pointer to the task that concluded
	FeatureExtractor *pTask = static_cast<FeatureExtractor*>(sender());
	m_lTasks.removeOne(pTask);
	delete pTask;

	// If all tasks concluded, indicate successful termination
	m_lVideoFiles.removeOne(sVideoFile);
	if(m_lVideoFiles.count() == 0)
		emit finished(0);
}