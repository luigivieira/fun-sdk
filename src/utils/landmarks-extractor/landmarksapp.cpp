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

#include "landmarksapp.h"
#include "version.h"
#include "landmarksextractiontask.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QThreadPool>
#include <QRegExp>

// +-----------------------------------------------------------
fsdk::LandmarksApp::LandmarksApp(int &argc, char **argv, const QString &sOrgName, const QString &sOrgDomain, const QString &sAppName, const QString &sAppVersion, const bool bUseSettings):
	Application(argc, argv, sOrgName, sOrgDomain, sAppName, sAppVersion, bUseSettings)
{
	// Replace the original message pattern from the parent class Application.
	// i.e.: - remove the source and line number from trace even in debug;
	//       - add the progress level instead of the log type;
	qSetMessagePattern("%{time yyyy.MM.dd h:mm:ss.zzz} [%{if-critical}l1%{endif}%{if-warning}l2%{endif}%{if-info}l3%{endif}%{if-debug}l4%{endif}]: %{message}");
	setLogLevel(Critical);
}

// +-----------------------------------------------------------
fsdk::LandmarksApp::CommandLineParseResult fsdk::LandmarksApp::parseCommandLine()
{
	//***************************************
	//* Sets up the command line parser
	//***************************************
	QCommandLineParser oParser;
	
	oParser.setApplicationDescription(tr("Tracks and extracts landmarks from video files of player's facial expressions."));
	oParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	// Input video file option
	oParser.addPositionalArgument("video file",
		tr("Video file to use for landmark extraction."),
		tr("<video file>")
	);

	// Ouutput CSV file option
	oParser.addPositionalArgument("csv file",
		tr("CSV file to create with the landmarks extracted."),
		tr("<csv file>")
	);

	// Messages level option
	QCommandLineOption oMsgLevelOpt(QStringList({"l", "level"}),
		tr("Sets the level of the messages to display in range: \n"
		   "1: only error messages (default)\n"
		   "2: error and warning messages\n"
		   "3: error, warning and info messages\n"
		   "4: error, warning, info and debug messages\n"
		), tr("1-4"), "1"
	);
	oParser.addOption(oMsgLevelOpt);

	// Automatic confirm overwrite option
	QCommandLineOption oAutoConfirmOpt(QStringList({ "y", "yes" }),
		tr("Automatically confirms the overwriting of the <csv file> if it already exists")
	);
	oParser.addOption(oAutoConfirmOpt);

	// Help and version options
	QCommandLineOption oHelpOpt = oParser.addHelpOption();
	QCommandLineOption oVersionOpt = oParser.addVersionOption();

	//***************************************
	//* Parse the arguments received
	//***************************************
	if(!oParser.parse(arguments()))
	{
		QString s = oParser.errorText();
		qCritical().noquote() << oParser.errorText() << endl;
		oParser.showHelp();
		return CommandLineError;
	}

	// Check if help was requested
	if(oParser.isSet(oHelpOpt))
	{
		oParser.showHelp();
		return CommandLineHelpRequested;
	}

	// Check if version was requested
	if(oParser.isSet(oVersionOpt))
	{
		oParser.showVersion();
		return CommandLineVersionRequested;
	}

	// Get the requested message level
	int iLevel = oParser.value(oMsgLevelOpt).toInt();
	if(iLevel < Critical || iLevel > Debug)
	{
		qCritical().noquote() << tr("invalid message level: %1").arg(iLevel) << endl;
		oParser.showHelp();
		return CommandLineError;
	}
	setLogLevel(static_cast<LogLevel>(iLevel));

	// Get the video and CSV files
	switch(oParser.positionalArguments().count())
	{
		case 0:
			qCritical().noquote() << tr("the arguments <video file> and <csv file> are required") << endl;
			oParser.showHelp();
			return CommandLineError;

		case 1:
			qCritical().noquote() << tr("the argument <csv file> is required") << endl;
			oParser.showHelp();
			return CommandLineError;

		case 2:
			break;

		default:
		{
			QStringList lUnknown;
			for(int i = 2; i < oParser.positionalArguments().count(); i++)
				lUnknown.append(oParser.positionalArguments()[i]);
			qCritical().noquote() << tr("unexpected arguments: %1").arg(lUnknown.join(' ')) << endl;
			oParser.showHelp();
			return CommandLineError;
		}
	}

	m_sVideoFile = oParser.positionalArguments().at(0);
	m_sCSVFile = oParser.positionalArguments().at(1);

	if(!QFile::exists(m_sVideoFile))
	{
		qCritical().noquote() << tr("video file does not exist: %1").arg(m_sVideoFile);
		return CommandLineError;
	}

	if(QFile::exists(m_sCSVFile) && !oParser.isSet(oAutoConfirmOpt))
	{
		qCritical().noquote() << tr("CSV file already exists: %1. Do you want to overwrite it [y/n]?").arg(m_sCSVFile);
		QTextStream oInput(stdin);
		QString sInput = oInput.read(1).toUpper();
		if(sInput != "Y")
			return CommandLineError;
	}

	QFile oTest(m_sCSVFile);
	if(!oTest.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		qCritical().noquote() << tr("CSV file is not writable: %1").arg(m_sCSVFile);
		return CommandLineError;
	}
	oTest.close();

	return CommandLineOk;
}

// +-----------------------------------------------------------
void fsdk::LandmarksApp::run()
{
	m_pTask = new LandmarksExtractionTask(m_sVideoFile);
	m_pTask->setAutoDelete(false);

	connect(m_pTask, &LandmarksExtractionTask::taskError, this, &LandmarksApp::taskError);
	connect(m_pTask, &LandmarksExtractionTask::taskProgress, this, &LandmarksApp::taskProgress);
	connect(m_pTask, &LandmarksExtractionTask::taskFinished, this, &LandmarksApp::taskFinished);
		
	qInfo().noquote() << tr("extraction of landmards from file %1 started.").arg(m_sVideoFile);
	QThreadPool::globalInstance()->start(m_pTask);
}

// +-----------------------------------------------------------
void fsdk::LandmarksApp::taskError(const QString &sVideoFile, const ExtractionTask::ExtractionError eError)
{
	QThreadPool::globalInstance()->waitForDone();
	disconnect(m_pTask, &LandmarksExtractionTask::taskError, this, &LandmarksApp::taskError);
	disconnect(m_pTask, &LandmarksExtractionTask::taskProgress, this, &LandmarksApp::taskProgress);
	disconnect(m_pTask, &LandmarksExtractionTask::taskFinished, this, &LandmarksApp::taskFinished);
	delete m_pTask;

	QFileInfo oFile;
	QString sLandmarks;

	switch(eError)
	{
		case LandmarksExtractionTask::InvalidVideoFile:
			qCritical().noquote() << tr("error reading input video file %1").arg(sVideoFile);
			exit(-1);
			break;

		case LandmarksExtractionTask::CancelRequested:
			qDebug().noquote() << tr("task for file %1 was cancelled").arg(sVideoFile);
			exit(-2);
			break;

		default:
			qCritical().noquote() << tr("unknown error while processing input video file %1").arg(sVideoFile);
			exit(-3);
			break;
	}
}

// +-----------------------------------------------------------
void fsdk::LandmarksApp::taskProgress(const QString &sVideoFile, int iProgress)
{
	qDebug().noquote() << tr("file %1 progress: %2%").arg(sVideoFile).arg(iProgress);
}

// +-----------------------------------------------------------
void fsdk::LandmarksApp::taskFinished(const QString &sVideoFile, const QVariant &vData)
{
	QThreadPool::globalInstance()->waitForDone();
	disconnect(m_pTask, &LandmarksExtractionTask::taskError, this, &LandmarksApp::taskError);
	disconnect(m_pTask, &LandmarksExtractionTask::taskProgress, this, &LandmarksApp::taskProgress);
	disconnect(m_pTask, &LandmarksExtractionTask::taskFinished, this, &LandmarksApp::taskFinished);
	delete m_pTask;

	if(!vData.value<LandmarksData>().saveToCSV(m_sCSVFile))
	{
		qCritical().noquote() << tr("error writing to CSV file %1").arg(m_sCSVFile);
		exit(-4);
	}
	else
	{
		qInfo().noquote() << tr("extraction of landmards from file %1 concluded.").arg(sVideoFile);
		exit(0);
	}
}

// +-----------------------------------------------------------
void fsdk::LandmarksApp::cancel()
{
	m_pTask->cancel();
}