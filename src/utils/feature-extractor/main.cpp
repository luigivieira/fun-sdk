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

#include "version.h"
#include <iostream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include "consoleapp.h"

using namespace std;
using namespace fsdk;

// +-----------------------------------------------------------
/**
 * Enumeration that defines the possible outcomes of the parsing
 * of the command line arguments.
 */
enum CommandLineParseResult
{
	/** The command line was parsed correctly with all required arguments. */
	CommandLineOk,
	
	/** There was an error in the parsing of the command line arguments. */
	CommandLineError,

	/** The user requested the application to display its version information. */
	CommandLineVersionRequested,

	/** The user requested the application to display its help information. */
	CommandLineHelpRequested
};

// +-----------------------------------------------------------
/**
 * Parses the command line arguments.
 * @param oParser Reference to the QCommandLineParser to help parsing the arguments.
 * @param sErrorMessage Reference to a QString to receive the error message in case
 * of errors are found while the arguments are parsed.
 * @param sInput Reference to a QString to receive the name of the video file or the
 * wildcard mask (for globbing) received from the command line.
 * @param sOutput Reference to a QString to receive the path and name of the CSV
 * file received from the command line.
 * @param bShowProgress Reference to a boolean to receive the indication to show or
 * not the tracking progress received from the command line.
 * @return One of the values in the CommandLineParseResult enum indicating
 * how the parsing of the command line proceeded.
 */
int parseCommandLine(QCommandLineParser &oParser, QString &sErrorMessage, QString &sInput, QString &sOutput, bool &bShowProgress)
{
	oParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	// Basic options: help and version
	QCommandLineOption oHelpOpt = oParser.addHelpOption();
	QCommandLineOption oVersionOpt = oParser.addVersionOption();

	// Parameter: Input video file
	oParser.addPositionalArgument("input", QCoreApplication::translate("main", "File name or wildcard pattern with the input video file(s) from where to perform the face tracking"), "<input file name or mask>");

	// Parameter: Output directory
	oParser.addPositionalArgument("output", QCoreApplication::translate("main", "Directory where the CSV files will be created"), "<output directory>");

	// Parameter: Boolean indication on if progress must be shown
	QCommandLineOption oShowProgressOpt(QStringList() << "p" << "progress",
		QCoreApplication::translate("main", "Displays the tracking progress"));
	oParser.addOption(oShowProgressOpt);

	// Parse the arguments given by the user
	if(!oParser.parse(QCoreApplication::arguments()))
	{
		sErrorMessage = oParser.errorText();
		return CommandLineError;
	}

	// Check values
	if(oParser.isSet(oHelpOpt))
		return CommandLineHelpRequested;
	else if(oParser.isSet(oVersionOpt))
		return CommandLineVersionRequested;
	else
	{
		switch(oParser.positionalArguments().count())
		{
			case 0:
				sErrorMessage = QCoreApplication::translate("main", "arguments %1 and %2 are required").arg("<input>").arg("<output>");
				return CommandLineError;

			case 1:
				sErrorMessage = QCoreApplication::translate("main", "argument %1 is required").arg("<output>");
				return CommandLineError;

			case 2:
				break;

			default:
				sErrorMessage = QCoreApplication::translate("main", "unknown arguments (only two required): %1").arg(oParser.positionalArguments().join(' '));
				return CommandLineError;
		}

		sInput = oParser.positionalArguments()[0];
		sOutput = oParser.positionalArguments()[1];
		bShowProgress = oParser.isSet(oShowProgressOpt);
		return CommandLineOk;
	}
}

/** Indication on whether the progress should be displayed (true) or not (false). */
bool g_bShowProgress;

// +-----------------------------------------------------------
/**
 * Captures and handles the application log messages.
 * @param eType QtMsgType enum value with the type of the log event.
 * @param oContext QMessageLogContext instance with information on where
 * the event happened (function, line, etc).
 * @param sMessage QString instance with the event message.
 */
void handleLogs(QtMsgType eType, const QMessageLogContext &oContext, const QString &sMessage)
{
	switch(eType)
	{
		case QtDebugMsg:
		case QtInfoMsg:
		case QtWarningMsg:
			if(g_bShowProgress)
				cout << sMessage.toStdString() << endl;
			break;

		case QtCriticalMsg:
		case QtFatalMsg:
			cerr << sMessage.toStdString() << endl;
			break;
	}
}

// +-----------------------------------------------------------
/**
 * Main function.
 * @param argc Integer with the number of arguments received from
 * the command line.
 * @param argv Array of strings with the arguments received from
 * the command line.
 * @return Integer with the exit code:
 *		0 indicates success
 *		1 indicates error in the command line call
 */

int main(int argc, char* argv[])
{
	// Application definition
	QCoreApplication oApp(argc, argv);

	QCoreApplication::setApplicationName(QCoreApplication::translate("main", "FSDK Face Tracker (Command Line)"));
	QCoreApplication::setApplicationVersion(FSDK_VERSION);

	qInstallMessageHandler(&handleLogs);

	// Parsing of command line options
	QCommandLineParser oParser;
	oParser.setApplicationDescription(QCoreApplication::translate("main", "Tracks facial landmarks in video streams and save them in a CSV file."));
	
	QString sErrorMsg, sInput, sOutput;
	switch(parseCommandLine(oParser, sErrorMsg, sInput, sOutput, g_bShowProgress))
	{
		case CommandLineOk:
			break;

		case CommandLineError:
			qCritical(qPrintable(sErrorMsg));
			oParser.showHelp();
			return 1;

		case CommandLineVersionRequested:
			oParser.showVersion();
			Q_UNREACHABLE();

		case CommandLineHelpRequested:
			oParser.showHelp();
			Q_UNREACHABLE();
	}

	// Processing

	QFileInfo oInput(sInput);

	QStringList lVideos;
	QDir oDir = oInput.dir();
	oDir.setNameFilters(QStringList(oInput.fileName()));
	foreach(QFileInfo oFile, oDir.entryInfoList())
		lVideos.append(oFile.absoluteFilePath());

	if(lVideos.count() == 0)
	{
		qCritical(qPrintable(QCoreApplication::translate("main", "invalid file name or wildcard mask: %1").arg(sInput)));
		oParser.showHelp();
		return -1;
	}

	QDir oOutput(sOutput);
	if(!oOutput.exists())
	{
		qCritical(qPrintable(QCoreApplication::translate("main", "invalid file name or wildcard mask: %1").arg(sInput)));
		oParser.showHelp();
		return -2;
	}

	ConsoleApp oConsoleApp(lVideos, sOutput);
	QObject::connect(&oConsoleApp, &ConsoleApp::finished, &oApp, &QCoreApplication::exit);
	oConsoleApp.run();
    
    return oApp.exec();
}