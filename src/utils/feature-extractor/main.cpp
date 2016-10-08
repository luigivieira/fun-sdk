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
#include <QTimer>

using namespace fsdk;

int main(int argc, char* argv[])
{
	RuntimeApp oApp(argc, argv, "University of Sao Paulo", "Fun SDK", "Feature Extractor", FSDK_VERSION);

	// Parse the command line
	switch(oApp.parseCommandLine())
	{
		case RuntimeApp::CommandLineError:
			return -1;

		case RuntimeApp::CommandLineVersionRequested:
		case RuntimeApp::CommandLineHelpRequested:
			return 0;

		case RuntimeApp::CommandLineOk:
		default:
			break;
	}

	// Schedule to run as soon as the event loop starts
	QTimer::singleShot(0, &oApp, SLOT(run()));

    return oApp.exec();
}