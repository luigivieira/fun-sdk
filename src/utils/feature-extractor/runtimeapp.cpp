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
#include "landmarksextractor.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QThreadPool>
#include <QRegExp>

 // +-----------------------------------------------------------
fsdk::RuntimeApp::RuntimeApp(int &argc, char **argv, const QString &sOrgName, const QString &sOrgDomain, const QString &sAppName, const QString &sAppVersion, const bool bUseSettings):
	Application(argc, argv, sOrgName, sOrgDomain, sAppName, sAppVersion, bUseSettings)
{
	// Replace the original message pattern from the parent class Application.
	// i.e.: - remove the source and line number from trace even in debug;
	//       - add the progress level instead of the log type;
	qSetMessagePattern("%{time yyyy.MM.dd h:mm:ss.zzz} [%{if-critical}p1%{endif}%{if-warning}p2%{endif}%{if-info}p3%{endif}%{if-debug}p4%{endif}]: %{message}");
	setLogLevel(Critical);
}

// +-----------------------------------------------------------
fsdk::RuntimeApp::CommandLineParseResult fsdk::RuntimeApp::parseCommandLine()
{
	//***************************************
	//* Sets up the command line parser
	//***************************************
	QCommandLineParser oParser;
	
	oParser.setApplicationDescription(tr("Extracts features from video files of player's facial expressions."));
	oParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	// Input video files option
	oParser.addPositionalArgument("video file(s)",
		tr("File name or wildcard pattern with the input video file(s) "
		   "from where to perform the extraction. The wildcard pattern "
		   "can use the ? and * symbols for respectively indicating one "
		   "or many characters, as well as a regular expression lists to "
		   "indicate *single character* alternatives.\n"
		   "For example, the wildcard 'player_03[2-7].mp4' will include "
		   "all files with the name starting with 'player_03', followed "
		   "by one single digit in the interval '2' to '7', and the '.mp4' "
		   "extension. Obs.: if you use a '|' in the regular expression "
		   "list, enclose all the wildcard mask in quotes to avoid that the "
		   "operating have it confused with a pipe."
		),
		tr("<video file> [<video file> [...]]")
	);

	// Generate landmarks option
	QCommandLineOption oLandmarksOpt(QStringList({"l", "landmarks"}),
		tr("Requests the extraction of facial landmarks from the video files. "
		   "The landmarks are saved to CSV text files at the given <target path> "
		   "and with the postfix '-landmarks' added to the original input file "
		   "name and the '.csv' extension."
		), tr("target path"), qApp->applicationDirPath()
	);
	oParser.addOption(oLandmarksOpt);

	// Show progress option
	QCommandLineOption oShowProgressOpt(QStringList({"p", "progress"}),
		tr("Sets the level of progress messages that shall be displayed. "
		   "The acceptable values of <level> are in range [1, 4], meaning: \n"
		   "1: display only critical error messages (default)\n"
		   "2: display critical and warning messages\n"
		   "3: display critical, warning and informational messages\n"
		   "4: display critical, warning, informational and debug messages\n"
		), tr("level"), "1"
	);
	oParser.addOption(oShowProgressOpt);

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

	// Get the requested log level
	int iLogLevel = oParser.value(oShowProgressOpt).toInt();
	if(iLogLevel < Critical || iLogLevel > Debug)
	{
		qCritical().noquote() << tr("invalid level of progress messages %1").arg(iLogLevel) << endl;
		oParser.showHelp();
		return CommandLineError;
	}
	setLogLevel(static_cast<LogLevel>(iLogLevel));

	// Check the possible actions requested
	bool bOk = false; // At least one action must have been specified

	// 1) extraction of landmarks
	if(oParser.isSet(oLandmarksOpt))
	{
		bOk = true;
		m_bExtractLandmarks = true;
		m_sLandmarksOutputDir = oParser.value(oLandmarksOpt);
		QDir oOutput(m_sLandmarksOutputDir);
		if(!oOutput.exists())
		{
			qCritical().noquote() << tr("the output directory for the extraction of landmarks does not exist: %1").arg(m_sLandmarksOutputDir) << endl;
			oParser.showHelp();
			return CommandLineError;
		}
	}

	// Get the input video files
	if(oParser.positionalArguments().count() == 0)
	{
		qCritical().noquote() << tr("at list one video file or wildcard mask is required") << endl;
		oParser.showHelp();
		return CommandLineError;
	}

	foreach(QString sInput, oParser.positionalArguments())
	{
		QFileInfo oInput(sInput);
		QDir oDir = oInput.dir();
		oDir.setNameFilters(QStringList(oInput.fileName()));

		if(oDir.entryInfoList().count() == 0)
		{
			QRegExp oRE("[?*\\[\\]]");
			if(sInput.contains(oRE))
				qCritical().noquote() << tr("input wildcard mask provided no file: ") << sInput << endl;
			else
				qCritical().noquote() << tr("input file does not exist: ") << sInput << endl;
			oParser.showHelp();
			return CommandLineError;
		}
		else
			foreach(QFileInfo oFile, oDir.entryInfoList())
				m_lVideoFiles.append(oFile.absoluteFilePath());
	}

	if(m_lVideoFiles.count() == 0) // Sanity check
	{
		qCritical().noquote() << tr("the input names and/or wildcard mask(s) provided did not return any video files: %1").arg(oParser.positionalArguments().join(' ')) << endl;
		oParser.showHelp();
		return CommandLineError;
	}

	if(!bOk)
	{
		qCritical().noquote() << tr("at least one action (-l, for instance) to perform on the input video files must be specified") << endl;
		oParser.showHelp();
		return CommandLineError;
	}

	return CommandLineOk;
}

// +-----------------------------------------------------------
void fsdk::RuntimeApp::run()
{
	// Run extractons tasks for each input file received from command line
	foreach(QString sFile, m_lVideoFiles)
	{
		QFileInfo oFile(sFile);
		LandmarksExtractor *pTask = new LandmarksExtractor(sFile);
		pTask->setAutoDelete(false);
		m_lTasks.append(pTask);

		connect(pTask, &LandmarksExtractor::taskError, this, &RuntimeApp::taskError);
		connect(pTask, &LandmarksExtractor::taskProgress, this, &RuntimeApp::taskProgress);
		connect(pTask, &LandmarksExtractor::taskFinished, this, &RuntimeApp::taskFinished);
		
		QThreadPool::globalInstance()->start(pTask);
	}
	quit();
}

// +-----------------------------------------------------------
void fsdk::RuntimeApp::taskError(const QString &sVideoFile, const ExtractionTask::ExtractionError eError)
{
	QFileInfo oFile;
	QString sLandmarks;

	switch(eError)
	{
		case LandmarksExtractor::InvalidVideoFile:
			qCritical().noquote() << tr("error reading input video file %1").arg(sVideoFile);
			break;

		case LandmarksExtractor::UnknownError:
			qCritical().noquote() << tr("unknown error while processing input video file %1").arg(sVideoFile);
			break;

		case LandmarksExtractor::CancelRequested:
			qDebug().noquote() << tr("(cancellation) task for file %1 stoped.").arg(sVideoFile);
			return; // The thread that requested the cancellation is handling the conclusion
	}

	// Delete the pointer to the task that generated error
	LandmarksExtractor *pErrorTask = static_cast<LandmarksExtractor*>(sender());
	m_lTasks.removeOne(pErrorTask);
	delete pErrorTask;

	// Signal the cancellation to all remaining tasks and wait for them to stop
	foreach(LandmarksExtractor *pTask, m_lTasks)
		pTask->cancel();
	QThreadPool::globalInstance()->waitForDone();

	// Delete the remaining pointers
	foreach(LandmarksExtractor *pTask, m_lTasks)
		delete pTask;
	m_lTasks.clear();
	
	// Signal the termination with the error code as the index of the task
	int iError = m_lVideoFiles.indexOf(sVideoFile) + 1;
	m_lVideoFiles.clear();
	exit(iError);
}

// +-----------------------------------------------------------
void fsdk::RuntimeApp::taskProgress(const QString &sVideoFile, int iProgress)
{
	qDebug().noquote() << tr("file %1 progress: %2%").arg(sVideoFile).arg(iProgress);
}

// +-----------------------------------------------------------
void fsdk::RuntimeApp::taskFinished(const QString &sVideoFile, const QVariant &vData)
{
	qInfo().noquote() << tr("file %1 done.").arg(sVideoFile);

	// Delete the pointer to the task that concluded
	LandmarksExtractor *pTask = static_cast<LandmarksExtractor*>(sender());
	m_lTasks.removeOne(pTask);
	delete pTask;

	// If all tasks concluded, indicate successful termination
	m_lVideoFiles.removeOne(sVideoFile);
	if(m_lVideoFiles.count() == 0)
		exit(0);
}