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

#include "runtimeapp.h"
#include "version.h"
#include <QTimer>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QThreadPool>
#include "featureextractor.h"

// +-----------------------------------------------------------
fsdk::RuntimeApp::RuntimeApp(int &argc, char **argv, const QString &sOrgName, const QString &sOrgDomain, const QString &sAppName, const QString &sAppVersion, const bool bUseSettings):
	Application(argc, argv, sOrgName, sOrgDomain, sAppName, sAppVersion, bUseSettings)
{
	// Schedule to run as soon as the event loop starts
	QTimer::singleShot(0, this, SLOT(run));
}

// +-----------------------------------------------------------
fsdk::RuntimeApp::CommandLineParseResult fsdk::RuntimeApp::parseCommandLine()
{
	// Setup the command line parser
	QCommandLineParser oParser;
	
	oParser.setApplicationDescription(tr("Extracts facial features from video files into CSV files."));
	oParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	QCommandLineOption oHelpOpt = oParser.addHelpOption();
	QCommandLineOption oVersionOpt = oParser.addVersionOption();

	oParser.addPositionalArgument("input", QCoreApplication::translate("main", "File name or wildcard pattern with the input video file(s) from where to perform the face tracking"), "<input file name or mask>");
	oParser.addPositionalArgument("output", QCoreApplication::translate("main", "Directory where the CSV files will be created"), "<output directory>");

	QCommandLineOption oShowProgressOpt(QStringList() << "p" << "progress",
		tr("Displays the tracking progress"));
	oParser.addOption(oShowProgressOpt);

	// Parse the arguments
	if(!oParser.parse(arguments()))
	{
		qCritical() << oParser.errorText();
		oParser.showHelp();
		return CommandLineError;
	}

	// Check received values
	if(oParser.isSet(oHelpOpt))
	{
		oParser.showHelp();
		return CommandLineHelpRequested;
	}
	else if(oParser.isSet(oVersionOpt))
	{
		oParser.showVersion();
		return CommandLineVersionRequested;
	}
	else
	{
		switch(oParser.positionalArguments().count())
		{
			case 0:
				qCritical() << tr("arguments %1 and %2 are required").arg("<input>").arg("<output>");
				return CommandLineError;

			case 1:
				qCritical() << tr("argument %1 is required").arg("<output>");
				return CommandLineError;

			case 2:
				break;

			default:
				qCritical() << tr("unknown arguments (only two required): %1").arg(oParser.positionalArguments().join(' '));
				return CommandLineError;
		}

		QString sInput = oParser.positionalArguments()[0];
		QString sOutput = oParser.positionalArguments()[1];

		return CommandLineOk;
	}
}

// +-----------------------------------------------------------
void fsdk::RuntimeApp::run()
{
	// Parse command line
	switch(parseCommandLine())
	{
		case CommandLineError:
			emit finished(-1);
			return;

		case CommandLineVersionRequested:
		case CommandLineHelpRequested:
			emit finished(0);
			return;

		case CommandLineOk:
		default:
			break;
	}

	// Run an extraction task for each input file
	foreach(QString sFile, m_lInputFiles)
	{
		QFileInfo oFile(sFile);
		QString sLandmarks = QString("%1/%2-landmarks.csv").arg(m_sOutputDir, oFile.baseName());
		FeatureExtractor *pTask = new FeatureExtractor(sFile, sLandmarks);
		pTask->setAutoDelete(false);
		m_lTasks.append(pTask);

		connect(pTask, &FeatureExtractor::error, this, &RuntimeApp::onTaskError);
		connect(pTask, &FeatureExtractor::progress, this, &RuntimeApp::onTaskProgress);
		connect(pTask, &FeatureExtractor::finished, this, &RuntimeApp::onTaskFinished);
		
		QThreadPool::globalInstance()->start(pTask);
	}
}

// +-----------------------------------------------------------
void fsdk::RuntimeApp::onTaskError(QString sVideoFile, FeatureExtractor::ExtractionError eError)
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
void fsdk::RuntimeApp::onTaskProgress(QString sVideoFile, int iProgress)
{
	qDebug(qPrintable(tr("file %1 progress: %2%%").arg(sVideoFile).arg(iProgress)));
}

// +-----------------------------------------------------------
void fsdk::RuntimeApp::onTaskFinished(QString sVideoFile)
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