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
#include "gabortestapp.h"
#include <QTimer>

using namespace fsdk;

/**
 * Main entry function.
 * @param argc Integer with the number of arguments
 * received from the command line.
 * @param argv Array of strings with the arguments received
 * from the command line.
 * @return Integer with the exit level.
 */
int main(int argc, char* argv[])
{
	GaborTestApp oApp(argc, argv, "University of Sao Paulo", "Fun SDK", "Project Information", FSDK_VERSION);

	// Parse the command line
	switch(oApp.parseCommandLine())
	{
		case GaborTestApp::CommandLineError:
			return -1;

		case GaborTestApp::CommandLineVersionRequested:
		case GaborTestApp::CommandLineHelpRequested:
			return 0;

		case GaborTestApp::CommandLineOk:
		default:
			break;
	}

	// Schedule to run as soon as the event loop starts
	QTimer::singleShot(0, &oApp, SLOT(run()));

    return oApp.exec();
}