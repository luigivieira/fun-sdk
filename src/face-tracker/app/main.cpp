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

#include "version.h"
#include "task.h"
#include <iostream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTimer>

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
 * @param sVideoFile Reference to a QString to receive the path and name of the video
 * file received from the command line.
 * @param sCSVFile Reference to a QString to receive the path and name of the CSV
 * file received from the command line.
 * @param bShowProgress Reference to a boolean to receive the indication to show or
 * not the tracking progress received from the command line.
 * @return One of the values in the CommandLineParseResult enum indicating
 * how the parsing of the command line proceeded.
 */
int parseCommandLine(QCommandLineParser &oParser, QString &sErrorMessage, QString &sVideoFile, QString &sCSVFile, bool &bShowProgress)
{
	oParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	// Basic options: help and version
	QCommandLineOption oHelpOpt = oParser.addHelpOption();
	QCommandLineOption oVersionOpt = oParser.addVersionOption();

	// Parameter: Input video file
	oParser.addPositionalArgument("input", QCoreApplication::translate("main", "Input file with the video from where to track the face"), "<input video file>");

	// Parameter: Output CSV file
	oParser.addPositionalArgument("output", QCoreApplication::translate("main", "Output CSV file with the landmarks tracked in the input video file"), "<output CSV file>");

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

		sVideoFile = oParser.positionalArguments()[0];
		sCSVFile = oParser.positionalArguments()[1];
		bShowProgress = oParser.isSet(oShowProgressOpt);
		return CommandLineOk;
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
	
	// Parsing of command line options
	QCommandLineParser oParser;
	oParser.setApplicationDescription(QCoreApplication::translate("main", "Tracks facial landmarks in video streams and save them in a CSV file."));
	
	QString sErrorMsg, sVideoFile, sCSVFile;
	bool bShowProgress;
	switch(parseCommandLine(oParser, sErrorMsg, sVideoFile, sCSVFile, bShowProgress))
	{
		case CommandLineOk:
			break;

		case CommandLineError:
			cerr << "(ERROR) " << qPrintable(sErrorMsg) << endl << endl;
			cout << qPrintable(oParser.helpText()) << endl;
			return 1;

		case CommandLineVersionRequested:
			oParser.showVersion();
			Q_UNREACHABLE();

		case CommandLineHelpRequested:
			oParser.showHelp();
			Q_UNREACHABLE();
	}

	// Processing
	QStringList lVideos;
	lVideos.append(sVideoFile);
	Task *pTask = new Task(lVideos, sCSVFile, &oApp);

	pTask->setShowProgress(bShowProgress);

	QObject::connect(pTask, &Task::finished, &oApp, &QCoreApplication::exit);
	QTimer::singleShot(0, pTask, &Task::run);
    
    return oApp.exec();
}