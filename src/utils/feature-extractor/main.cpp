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
#include "runtimeapp.h"
#include <iostream>
#include <QDir>

using namespace std;
using namespace fsdk;

// +-----------------------------------------------------------


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


/** Indication on whether the progress should be displayed (true) or not (false). */
bool g_bShowProgress;

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
	RuntimeApp oApp(argc, argv, "University of Sao Paulo", "Fun SDK", "Feature Extractor", FSDK_VERSION);

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