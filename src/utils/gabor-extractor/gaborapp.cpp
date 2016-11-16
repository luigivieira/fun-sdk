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

#include "gaborapp.h"
#include "version.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QThreadPool>
#include <QRegExp>
#include "naming.h"
#include <QRegularExpression>
#include "gaborbank.h"
#include "imageman.h"

// To allow using _getch()/getch() for reading the overwrite confirmation answer
#ifdef WIN32
#include <conio.h>
#else
#include <curses.h>
#endif

// +-----------------------------------------------------------
fsdk::GaborApp::GaborApp(int &argc, char **argv, const QString &sOrgName, const QString &sOrgDomain, const QString &sAppName, const QString &sAppVersion, const bool bUseSettings):
	Application(argc, argv, sOrgName, sOrgDomain, sAppName, sAppVersion, bUseSettings)
{
	// Replace the original message pattern from the parent class Application.
	// i.e.: - remove the source and line number from trace even in debug;
	//       - add the progress level instead of the log type;
	qSetMessagePattern("%{time yyyy.MM.dd h:mm:ss.zzz} [%{if-critical}l1%{endif}%{if-warning}l2%{endif}%{if-info}l3%{endif}%{if-debug}l4%{endif}]: %{message}");
	setLogLevel(Critical);
}

// +-----------------------------------------------------------
fsdk::GaborApp::CommandLineParseResult fsdk::GaborApp::parseCommandLine()
{
	//***************************************
	//* Sets up the command line parser
	//***************************************
	QCommandLineParser oParser;
	
	oParser.setApplicationDescription(tr("Extracts Gabor responses from image or video files of facial expressions."));
	oParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	// Input file option
	oParser.addPositionalArgument("input file",
		tr("Image or video file (wildcard masks can be used) to use for the Gabor responses extraction."),
		tr("<input file>")
	);

	// Landmarks file option
	oParser.addPositionalArgument("landmarks file",
		tr("CSV file (wildcard masks can be used) with the facial landmarks in the input file."),
		tr("<landmarks file>")
	);

	// Output file option
	oParser.addPositionalArgument("output file",
		tr("CSV file (or wildcard mask) to create with the Gabor responses extracted."),
		tr("<output file>")
	);

	// Messages level option
	QCommandLineOption oMsgLevelOpt(QStringList({"l", "level"}),
		tr("Level of the messages to display, in range [1,4]: \n"
		   "1: only error messages (default).\n"
		   "2: error and warning messages.\n"
		   "3: error, warning and info messages.\n"
		   "4: error, warning, info and progress messages.\n"
		), tr("value"), "1"
	);
	oParser.addOption(oMsgLevelOpt);

	// Automatic confirm overwrite option
	QCommandLineOption oAutoConfirmOpt(QStringList({ "y", "yes" }),
		tr("Automatically confirms the overwriting of the <csv file> if it already exists.")
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
	QRegularExpression oRELevel("^[1-4]$");
	bool bValid = oRELevel.match(oParser.value(oMsgLevelOpt)).hasMatch();
	if(!bValid)
	{
		qCritical().noquote() << tr("invalid message level: %1").arg(oParser.value(oMsgLevelOpt)) << endl;
		oParser.showHelp();
		return CommandLineError;
	}
	int iLevel = oParser.value(oMsgLevelOpt).toInt();
	setLogLevel(static_cast<LogLevel>(iLevel));

	// Check the input and landmark files, as well as the CSV files (or wildcards) arguments
	switch(oParser.positionalArguments().count())
	{
		case 0:
			qCritical().noquote() << tr("the arguments <input file>, <landmarks file> and <csv file> are required") << endl;
			oParser.showHelp();
			return CommandLineError;

		case 1:
			qCritical().noquote() << tr("the arguments <landmarks file> and <csv file> are required") << endl;
			oParser.showHelp();
			return CommandLineError;

		case 2:
			qCritical().noquote() << tr("the argument <csv file> is required") << endl;
			oParser.showHelp();
			return CommandLineError;

		case 3:
			break;

		default:
		{
			QStringList lUnknown;
			for(int i = 3; i < oParser.positionalArguments().count(); i++)
				lUnknown.append(oParser.positionalArguments()[i]);
			qCritical().noquote() << tr("unexpected arguments: %1").arg(lUnknown.join(' ')) << endl;
			oParser.showHelp();
			return CommandLineError;
		}
	}

	// Map the input image/video/wildcard to the landmarks and CSV file/wildcard
	QString sInputFile = oParser.positionalArguments().at(0);
	QString sLandmarksFile = oParser.positionalArguments().at(1);
	QString sCSVFile = oParser.positionalArguments().at(2);
	
	QMap<QString, QString> mMapping[2];
	Naming::WildcardListingReturn aRet[2];
	aRet[0] = Naming::wildcardListing(sInputFile, sLandmarksFile, mMapping[0]);
	aRet[1] = Naming::wildcardListing(sLandmarksFile, sCSVFile, mMapping[1]);

	for(int i = 0; i < 2; i++)
	{
		QString sSrc = (i == 0 ? sInputFile : sLandmarksFile);
		QString sTgt = (i == 0 ? sLandmarksFile : sCSVFile);

		switch(aRet[i])
		{
			case Naming::InvalidSourceWildcard:
				qCritical().noquote() << tr("invalid wildcard for the file: %1").arg(sSrc) << endl;
				oParser.showHelp();
				return CommandLineError;

			case Naming::InvalidTargetWildcard:
				qCritical().noquote() << tr("invalid wildcard for the file: %1").arg(sTgt) << endl;
				oParser.showHelp();
				return CommandLineError;

			case Naming::DifferentWildcards:
				qCritical().noquote() << tr("the wildcards between file %1 and %2 must be the same").arg(sSrc).arg(sTgt) << endl;
				oParser.showHelp();
				return CommandLineError;

			case Naming::FileNotExist:
				qCritical().noquote() << tr("file does not exist: %1").arg(sSrc) << endl;
				oParser.showHelp();
				return CommandLineError;

			case Naming::EmptySourceListing:
				qCritical().noquote() << tr("file wildcard did not return any file: %1").arg(sSrc) << endl;
				oParser.showHelp();
				return CommandLineError;

			case Naming::ListingOk:
				break;
		}
	}

	if(mMapping[0].count() != mMapping[1].count()) // Sanity check
	{
		qCritical().noquote() << tr("something unexpected happened when processing the wildcards in command line");
		return CommandLineError;
	}

	// Add the files to the final task map
	m_mTaskFiles.clear();
	for(int i = 0; i < mMapping[0].count(); i++)
	{
		QString sInput = (mMapping[0].cbegin() + i).key();
		QString sLandmarks = (mMapping[0].cbegin() + i).value();
		QString sCSV = (mMapping[1].cbegin() + i).value();
		m_mTaskFiles[sInput] = TaskPair(sLandmarks, sCSV);
	}

	// Check if existing CSV files should be overwriting
	QStringList lIgnored;
	bool bAutoConfirm = oParser.isSet(oAutoConfirmOpt);
	QMap<QString, TaskPair>::const_iterator it;
	for(it = m_mTaskFiles.cbegin(); it != m_mTaskFiles.cend(); ++it)
	{
		QString sInputFile = it.key();
		QString sCSVFile = it.value().second;

		if(!confirmOverwrite(sCSVFile, bAutoConfirm))
			lIgnored.append(sInputFile);
	}
	// Remove the tasks that would overwrite a CSV file and were denied by the user
	foreach(QString sIgnored, lIgnored)
		m_mTaskFiles.remove(sIgnored);

	// Check if remaining target CSV files can be opened with read-write access
	bAutoConfirm = oParser.isSet(oAutoConfirmOpt);
	lIgnored.clear();
	for(it = m_mTaskFiles.cbegin(); it != m_mTaskFiles.cend(); ++it)
	{
		QString sInputFile = it.key();
		QString sCSVFile = it.value().second;

		bool bCancel;
		if(!confirmWritable(sCSVFile, bAutoConfirm, bCancel))
		{
			if(bCancel)
			{
				qCritical().noquote() << tr("cancelled due to not writable CSV files");
				return CommandLineError;
			}
			else
				lIgnored.append(sInputFile);
		}		
	}
	// Remove the tasks with CSV files that can not be opened with read-write access
	foreach(QString sIgnored, lIgnored)
	{
		qDebug().noquote() << tr("Ignoring input file %1 because target CSV %2 is not writable").arg(sIgnored, m_mTaskFiles[sIgnored].second);
		m_mTaskFiles.remove(sIgnored);
	}

	// If no tasks remain, terminate with error
	if(m_mTaskFiles.count() == 0)
		return CommandLineError;

	// Otherwise, go on! :)
	return CommandLineOk;
}

// +-----------------------------------------------------------
bool fsdk::GaborApp::confirmOverwrite(const QString sCSVFilename, bool &bAutoConfirm)
{
	// If the file does not exist, it certainly can be overwritten! :)
	if(!QFile::exists(sCSVFilename))
		return true;

	// If autoconfirm is set, just confirm overwriting the file!
	if(bAutoConfirm)
		return true;

	// Otherwise, ask the user
	QString sNo = tr("(n)o");
	QString sYes = tr("(y)es");
	QString sAll = tr("yes to (a)ll");

	qCritical().noquote() << tr("CSV file already exists: %1. Do you want to overwrite it [%2/%3/%4]?").arg(sCSVFilename, sNo, sYes, sAll);

	QRegularExpression oRE("(?<=\\().(?=\\))");

	QChar cNo = oRE.match(sNo).captured(0).at(0).toUpper();
	QChar cYes = oRE.match(sYes).captured(0).at(0).toUpper();
	QChar cAll = oRE.match(sAll).captured(0).at(0).toUpper();
	QChar cAnswer;
	while(true)
	{
#ifdef WIN32
		cAnswer = QChar(_getch()).toUpper();
#else
		cAnswer = QChar(getch()).toUpper();
#endif
		if(cAnswer == cNo)
			return false;
		else if(cAnswer == cYes)
			return true;			
		else if(cAnswer == cAll)
		{
			bAutoConfirm = true;
			return true;
		}
		else
			std::cout << "\a"; // Send a "beep" to the console
	}
}

// +-----------------------------------------------------------
bool fsdk::GaborApp::confirmWritable(const QString sCSVFilename, bool &bAutoIgnore, bool &bCancel)
{
	bCancel = false;

	QFile oTest(sCSVFilename);
	bool bWritable = oTest.open(QIODevice::ReadWrite | QIODevice::Text);
	oTest.close();

	// If the file is writable, nothing to do!
	if(bWritable)
		return true;

	// If autoignore is set, ignore the file!
	if(bAutoIgnore)
		return false;

	// Otherwise, ask the user what to do
	QString sCancel = tr("(c)ancel");
	QString sIgnore = tr("(i)gnore");
	QString sIgnoreAll = tr("ignore (a)ll");

	qCritical().noquote() << tr("CSV file can not be created or opened for writing: %1. What do you want to do? [%2/%3/%4]?").arg(sCSVFilename, sCancel, sIgnore, sIgnoreAll);

	QRegularExpression oRE("(?<=\\().(?=\\))");

	QChar cCancel = oRE.match(sCancel).captured(0).at(0).toUpper();
	QChar cIgnore = oRE.match(sIgnore).captured(0).at(0).toUpper();
	QChar cIgnoreAll = oRE.match(sIgnoreAll).captured(0).at(0).toUpper();
	QChar cAnswer;
	while(true)
	{
#ifdef WIN32
		cAnswer = QChar(_getch()).toUpper();
#else
		cAnswer = QChar(getch()).toUpper();
#endif
		if(cAnswer == cCancel)
		{
			bCancel = true;
			return false;
		}
		else if(cAnswer == cIgnore)
			return false;
		else if(cAnswer == cIgnoreAll)
		{
			bAutoIgnore = true;
			return false;
		}
		else
			std::cout << "\a"; // Send a "beep" to the console
	}
}

// +-----------------------------------------------------------
fsdk::GaborExtractionTask* fsdk::GaborApp::createTask(const QString &sInputFile, const QString &sLandmarksFile)
{
	GaborExtractionTask *pTask = new GaborExtractionTask(sInputFile, sLandmarksFile);
	pTask->setAutoDelete(false);
	m_lTasks.append(pTask);

	connect(pTask, &GaborExtractionTask::taskError, this, &GaborApp::taskError);
	connect(pTask, &GaborExtractionTask::taskProgress, this, &GaborApp::taskProgress);
	connect(pTask, &GaborExtractionTask::taskFinished, this, &GaborApp::taskFinished);

	return pTask;
}

// +-----------------------------------------------------------
void fsdk::GaborApp::deleteTask(fsdk::GaborExtractionTask* pTask)
{
	m_lTasks.removeOne(pTask);

	disconnect(pTask, &GaborExtractionTask::taskError, this, &GaborApp::taskError);
	disconnect(pTask, &GaborExtractionTask::taskProgress, this, &GaborApp::taskProgress);
	disconnect(pTask, &GaborExtractionTask::taskFinished, this, &GaborApp::taskFinished);

	delete pTask;
}

// +-----------------------------------------------------------
void fsdk::GaborApp::run()
{
	QMap<QString, TaskPair>::const_iterator it;
	for(it = m_mTaskFiles.cbegin(); it != m_mTaskFiles.cend(); ++it)
	{
		QString sInputFile = it.key();
		QString sLandmarksFile = it.value().first;
		GaborExtractionTask *pTask = createTask(sInputFile, sLandmarksFile);
		QThreadPool::globalInstance()->start(pTask);
	}
}

// +-----------------------------------------------------------
void fsdk::GaborApp::taskError(const QString &sInputFile, const ExtractionTask::ExtractionError eError)
{
	switch(eError)
	{
		case GaborExtractionTask::InvalidInputFile:
			qCritical().noquote() << tr("error reading input file %1").arg(sInputFile);
			break;

		case GaborExtractionTask::CancelRequested:
			qDebug().noquote() << tr("task for file file %1 was cancelled").arg(sInputFile);
			break;

		default:
			qCritical().noquote() << tr("unknown error while processing input file %1").arg(sInputFile);
			break;
	}

	GaborExtractionTask *pTask = static_cast<GaborExtractionTask*>(sender());
	deleteTask(pTask);

	if(m_lTasks.count() == 0)
	{
		QThreadPool::globalInstance()->waitForDone();
		exit(-2);
	}
}

// +-----------------------------------------------------------
void fsdk::GaborApp::taskProgress(const QString &sInputFile, int iProgress)
{
	qDebug().noquote() << tr("file %1 progress: %2%").arg(sInputFile).arg(iProgress);
}

// +-----------------------------------------------------------
void fsdk::GaborApp::taskFinished(const QString &sInputFile, const QVariant &vData)
{
	GaborExtractionTask *pTask = static_cast<GaborExtractionTask*>(sender());
	deleteTask(pTask);

	int iRet;
	QString sCSVFile = m_mTaskFiles[sInputFile].second;

	if(!vData.value<GaborData>().saveToCSV(sCSVFile))
	{
		qCritical().noquote() << tr("error writing to CSV file %1").arg(sCSVFile);
		iRet = -3;
	}
	else
	{
		qInfo().noquote() << tr("extraction of landmards from file %1 concluded.").arg(sInputFile);
		iRet = 0;
	}

	if(m_lTasks.count() == 0)
	{
		QThreadPool::globalInstance()->waitForDone();
		exit(iRet);
	}
}

// +-----------------------------------------------------------
void fsdk::GaborApp::cancel()
{
	foreach(ExtractionTask *pTask, m_lTasks)
		pTask->cancel();
}