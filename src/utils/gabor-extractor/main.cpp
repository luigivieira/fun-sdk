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
#include "gaborapp.h"
#include <QTimer>
#include <signal.h>

#include "gaborbank.h"
#include <iomanip>

using namespace fsdk;

/* Global variable used to allow indicating the cancelation request to the application. */
GaborApp *g_pApp = NULL;

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
	GaborKernel oKernel(CV_PI/4, 3);
	namedWindow("Kernel", WINDOW_AUTOSIZE);
	imshow("Kernel", oKernel.buildThumbnail(200));

	std::cout << "Size: " << std::setprecision(2) << oKernel.size() << std::endl;
	std::cout << "Sigma: " << std::setprecision(2) << oKernel.sigma() << std::endl;
	std::cout << "Lambda: " << std::setprecision(2) << oKernel.lambda() << std::endl;
	std::cout << "Theta: " << std::setprecision(2) << oKernel.theta() << std::endl;
	std::cout << "Psi: " << std::setprecision(2) << oKernel.psi() << std::endl;

	Mat oTeste = imread("C:/temp/teste/teste3.png", 0);
	oTeste = oKernel.filter(oTeste);
	
	normalize(oTeste, oTeste, 0, 255, NORM_MINMAX, CV_8UC1);
	//oTeste.convertTo(oTeste, CV_8UC1);
	namedWindow("Imagem", WINDOW_AUTOSIZE);
	imshow("Imagem", oTeste);

	/*Mat oReal, oImag;
	GaborBank oBank;
	oReal = oBank.buildThumbnail(64, GaborKernel::RealComp);
	oImag = oBank.buildThumbnail(64, GaborKernel::ImaginaryComp);

	namedWindow("Real", WINDOW_AUTOSIZE);
	namedWindow("Imaginary", WINDOW_AUTOSIZE);

	imshow("Real", oReal);
	imshow("Imaginary", oImag);*/

	waitKey(0);

	return 0;

	(void)signal(SIGINT, handleSigTerm);

	g_pApp = new GaborApp(argc, argv, "University of Sao Paulo", "Fun SDK", "Feature Extractor", FSDK_VERSION);

	// Parse the command line
	switch(g_pApp->parseCommandLine())
	{
		case GaborApp::CommandLineError:
			return -1;

		case GaborApp::CommandLineVersionRequested:
		case GaborApp::CommandLineHelpRequested:
			return 0;

		case GaborApp::CommandLineOk:
		default:
			break;
	}

	// Schedule to run as soon as the event loop starts
	QTimer::singleShot(0, g_pApp, SLOT(run()));

    return g_pApp->exec();
}