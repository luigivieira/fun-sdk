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
#include "landmarksapp.h"
#include <QTimer>
#include <signal.h>

using namespace fsdk;

/* Global variable used to allow indicating the cancelation request to the application. */
LandmarksApp *g_pApp = NULL;

/**
 * Handles the sigterm signal (ctrl+c) to allow
 * cancelling the execution of the task.
 * @param iSignum Integer with the number of the signal captured.
 */
void handleSigTerm(int iSignum)
{
	Q_UNUSED(iSignum);
	g_pApp->cancel();
	(void)signal(SIGINT, SIG_DFL);
}

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
	(void)signal(SIGINT, handleSigTerm);

	g_pApp = new LandmarksApp(argc, argv, "University of Sao Paulo", "Fun SDK", "Landmarks Extractor", FSDK_VERSION);

	// Parse the command line
	switch(g_pApp->parseCommandLine())
	{
		case LandmarksApp::CommandLineError:
			return -1;

		case LandmarksApp::CommandLineVersionRequested:
		case LandmarksApp::CommandLineHelpRequested:
			return 0;

		case LandmarksApp::CommandLineOk:
		default:
			break;
	}

	// Schedule to run as soon as the event loop starts
	QTimer::singleShot(0, g_pApp, SLOT(run()));

    return g_pApp->exec();
}