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

#include "gabortestapp.h"
#include "version.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include "gaborbank.h"
#include "imageman.h"
#include <opencv2\opencv.hpp>
#include <QApplication>

using namespace cv;

// +-----------------------------------------------------------
fsdk::GaborTestApp::GaborTestApp(int &argc, char **argv, const QString &sOrgName, const QString &sOrgDomain, const QString &sAppName, const QString &sAppVersion, const bool bUseSettings):
	Application(argc, argv, sOrgName, sOrgDomain, sAppName, sAppVersion, bUseSettings)
{
	// Replace the original message pattern from the parent class Application.
	// i.e.: - remove the source and line number from trace even in debug;
	//       - add the progress level instead of the log type;
	qSetMessagePattern("%{time yyyy.MM.dd h:mm:ss.zzz} [%{if-critical}l1%{endif}%{if-warning}l2%{endif}%{if-info}l3%{endif}%{if-debug}l4%{endif}]: %{message}");
	setLogLevel(Critical);
}

// +-----------------------------------------------------------
fsdk::GaborTestApp::CommandLineParseResult fsdk::GaborTestApp::parseCommandLine()
{
	//***************************************
	//* Sets up the command line parser
	//***************************************
	QCommandLineParser oParser;
	
	oParser.setApplicationDescription(tr("Testing utility of Gabor filters."));
	oParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	QCommandLineOption oLambdaOpt(QStringList({ "w", "wavelength" }),
		tr("Sets the wavelength in pixels of the sinusoidal carrier in a Gabor kernel. "
			"The minimum value accepted is 3. To set more than one wavelength, "
			"separate their values with commas (example: -w 5,11,22). This argument "
			"is mandatory to use custom Gabor kernels."
		), tr("value")
	);
	oParser.addOption(oLambdaOpt);

	QCommandLineOption oThetaOpt(QStringList({ "o", "orientation" }),
		tr("Sets the orientation in degrees of the sinusoidal carrier in a Gabor kernel. "
			"Accepted values are in range [0, 180]. To set more than one orientation, "
			"separate their values with commas (example: -o 0,45,90). This argument "
			"is mandatory to use custom Gabor kernels."
		), tr("value")
	);
	oParser.addOption(oThetaOpt);

	QCommandLineOption oSigmaOpt(QStringList({ "d", "deviation" }),
		tr("Sets the standard deviation in pixels of the Gaussian envelope in a Gabor kernel. "
			"The minimum value accepted is 3. To set more than one deviation, "
			"separate their values with commas (example: -d 12,45). If this argument "
			"is not provided or if it has less values than informed in wavelenght/orientation, "
			"the missing values will be automatically calculated to limit the cutoff to 0.5%. "
		), tr("value")
	);
	oParser.addOption(oSigmaOpt);

	QCommandLineOption oPsiOpt(QStringList({ "p", "phase" }),
		tr("Sets the phase offset in degrees of the sinusioidal carrier in a Gabor kernel. "
			"The minimum value accepted is 0. To set more than one phase, "
			"separate their values with commas (example: -p 0,45). If this argument "
			"is not provided or if it has less values than informed in wavelength/orientation, "
			"the missing values will use the default of 90."
		), tr("value")
	);
	oParser.addOption(oPsiOpt);

	QCommandLineOption oWindowSizeOpt(QStringList({ "s", "size" }),
		tr("Sets the window size in pixels of a Gabor kernel. "
			"The minimum value accepted is 3. To set more than one window size, "
			"separate their values with commas (example: -s 3,9). If this argument "
			"is not provided or if it has less values than informed in wavelength/orientation, "
			"the missing values will be automatically calculated from the deviation value. "
			"Obs.: the same value is used for both the width and height of a kernel because "
			"this implementation only supports non-elliptical Gaussian envelopes (spatial aspect "
			"ratio = 1)."
		), tr("value")
	);
	oParser.addOption(oWindowSizeOpt);

	QCommandLineOption oFileOpt(QStringList({ "f", "file" }),
		tr("Instead of displaying the result image in a graphical window, "
			"saves the it to the given image file (supported formats include"
			"BMP, PNG, JPEG and TIFF, automatically detected from the file extension)."
		), tr("file")
	);
	oParser.addOption(oFileOpt);

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

	//***************************************
	//* Handle the argument values
	//***************************************

	// Process the Lambda parameters
	if(oParser.isSet(oLambdaOpt))
	{
		QStringList lValues = oParser.value(oLambdaOpt).split(',');
		foreach(QString sValue, lValues)
		{
			bool bOk;
			double dLambda = sValue.toDouble(&bOk);
			if(!bOk || dLambda < 3)
			{
				qCritical().noquote() << tr("invalid wavelength value %1").arg(sValue) << endl;
				return CommandLineError;
			}
			m_lLambda.append(dLambda);
		}
	}

	// Process the Theta parameters
	if(oParser.isSet(oThetaOpt))
	{
		QStringList lValues = oParser.value(oThetaOpt).split(',');
		foreach(QString sValue, lValues)
		{
			bool bOk;
			double dTheta = sValue.toDouble(&bOk);
			if(!bOk || dTheta < 0 || dTheta > 180)
			{
				qCritical().noquote() << tr("invalid orientation value %1").arg(sValue) << endl;
				return CommandLineError;
			}
			m_lTheta.append(degreesToRadians(dTheta));
		}
	}

	if((oParser.isSet(oLambdaOpt) || oParser.isSet(oThetaOpt)) && (m_lLambda.count() != m_lTheta.count()))
	{
		qCritical().noquote() << tr("the wavelength and orientation must have the same number of values.") << endl;
		return CommandLineError;
	}

	// Process the Sigma parameters
	if(oParser.isSet(oSigmaOpt))
	{
		QStringList lValues = oParser.value(oSigmaOpt).split(',');
		foreach(QString sValue, lValues)
		{
			bool bOk;
			double dSigma = sValue.toDouble(&bOk);
			if(!bOk || dSigma < 0)
			{
				qCritical().noquote() << tr("invalid standard deviation value %1").arg(sValue) << endl;
				return CommandLineError;
			}
			m_lSigma.append(dSigma);
		}
	}
	if(m_lSigma.count() > m_lLambda.count())
	{
		qCritical().noquote() << tr("too much standard deviation values were informed %1").arg(oParser.value(oSigmaOpt)) << endl;
		return CommandLineError;
	}

	//  Process the Psi parameters
	if(oParser.isSet(oPsiOpt))
	{
		QStringList lValues = oParser.value(oPsiOpt).split(',');
		foreach(QString sValue, lValues)
		{
			bool bOk;
			double dPsi = sValue.toDouble(&bOk);
			if(!bOk || dPsi < 0)
			{
				qCritical().noquote() << tr("invalid phase offset value %1").arg(sValue) << endl;
				oParser.showHelp();
				return CommandLineError;
			}
			m_lPsi.append(degreesToRadians(dPsi));
		}
	}
	if(m_lPsi.count() > m_lLambda.count())
	{
		qCritical().noquote() << tr("too much phase offset values were informed %1").arg(oParser.value(oPsiOpt)) << endl;
		return CommandLineError;
	}

	// Process the WindowSize parameters
	if(oParser.isSet(oWindowSizeOpt))
	{
		QStringList lValues = oParser.value(oWindowSizeOpt).split(',');
		foreach(QString sValue, lValues)
		{
			bool bOk;
			int iWindowSize = sValue.toInt(&bOk);
			if(!bOk || iWindowSize < 3)
			{
				qCritical().noquote() << tr("invalid window size value %1").arg(sValue) << endl;
				oParser.showHelp();
				return CommandLineError;
			}
			m_lWindowSize.append(iWindowSize);
		}
	}
	if(m_lWindowSize.count() > m_lLambda.count())
	{
		qCritical().noquote() << tr("too much window size values were informed %1").arg(oParser.value(oWindowSizeOpt)) << endl;
		return CommandLineError;
	}

	// Process the save-to-file option
	if(oParser.isSet(oFileOpt))
		m_sSaveImageFilename = oParser.value(oFileOpt);

	return CommandLineOk;
}

// +-----------------------------------------------------------
void fsdk::GaborTestApp::run()
{
	int iRet = 0;

	GaborBank oBank;
	if(m_lLambda.count() == 0)
		oBank = GaborBank::defaultBank();
	else
	{
		for(int i = 0; i < m_lLambda.count(); i++)
		{
			double dLambda = m_lLambda[i];
			double dTheta = m_lTheta[i];
			double dPsi = i < m_lPsi.count() ? m_lPsi[i] : CV_PI / 2;

			GaborKernel oKernel(dTheta, dLambda, dPsi);
			if(i < m_lSigma.count())
				oKernel.setSigma(m_lSigma[i]);
			if(i < m_lWindowSize.count())
				oKernel.setWindowSize(m_lWindowSize[i]);

			oBank.addKernel(oKernel);
		}
	}

	QString sWindowName = QApplication::translate("GaborBank", "Gabor Bank");
	namedWindow(sWindowName.toStdString(), WINDOW_AUTOSIZE);
	if(oBank.kernels().count() == 1)
		imshow(sWindowName.toStdString(), oBank.kernels()[0].getThumbnail(GaborKernel::RealComp, Size(216, 216), true));
	else
		imshow(sWindowName.toStdString(), oBank.getThumbnails());
	waitKey(0);

	/*if(m_bBankDisplay)
	{
		if(!displayGaborBank(m_sSaveImageFilename))
		{
			qCritical().noquote() << tr("Error writing to file %1").arg(m_sSaveImageFilename);
			iRet = -2;
		}
	}

	if(m_bKernelGen)
	{
		if(!displayKernel(m_sSaveImageFilename))
		{
			qCritical().noquote() << tr("Error writing to file %1").arg(m_sSaveImageFilename);
			iRet = -2;
		}
	}*/

	exit(iRet);
}

// +-----------------------------------------------------------
double fsdk::GaborTestApp::degreesToRadians(const double dValue) const
{
	return dValue * CV_PI / 180;
}

// +-----------------------------------------------------------
bool fsdk::GaborTestApp::displayGaborBank(const QString &sFilename) const
{
	//if(sFilename.isEmpty())
	//{
		QString sWindowName = QApplication::translate("GaborBank", "Project's Gabor Bank");
		namedWindow(sWindowName.toStdString(), WINDOW_AUTOSIZE);
		//imshow(sWindowName.toStdString(), m_pBank->getThumbnails());
		waitKey(0);
		return true;
	//}
	//else
		//return imwrite(sFilename.toStdString(), m_pBank->getThumbnails());
}

// +-----------------------------------------------------------
bool fsdk::GaborTestApp::displayKernel(const QString &sFilename) const
{
	/*Mat oImage = m_pKernel->getThumbnail(GaborKernel::RealComp, Size(256, 256), true);
	if(sFilename.isEmpty())
	{
		QString sWindowName = QApplication::translate("GaborBank", "Gabor Kernel");
		namedWindow(sWindowName.toStdString(), WINDOW_AUTOSIZE);
		imshow(sWindowName.toStdString(), oImage);
		waitKey(0);
		return true;
	}
	else
		return imwrite(sFilename.toStdString(), oImage);*/

	return true;
}

// +-----------------------------------------------------------
bool fsdk::GaborTestApp::testGaborBank(const QString &sFilename) const
{
	// Read the image file
	Mat oTestImage = imread(sFilename.toStdString(), CV_LOAD_IMAGE_COLOR);
	if(oTestImage.empty())
		return false;

	// Filter the image with the Gabor bank
	GaborBank oBank;
	QMap<KernelParameters, cv::Mat> mResponses;
	QMap<KernelParameters, cv::Mat> mReal;
	QMap<KernelParameters, cv::Mat> mImag;
	oBank.filter(oTestImage, mResponses, mReal, mImag);

	// Produce a collated image with the responses
	QList<Mat> lResponses;
	QStringList lXLabels, lYLabels;
	QMap<KernelParameters, cv::Mat>::const_iterator it;
	for(it = mResponses.cbegin(); it != mResponses.cend(); ++it)
	{
		KernelParameters oParams = it.key();
		Mat oResponses = it.value();

		double dLambda = oParams.first;
		double dTheta = oParams.second;
		QString sLambda, sTheta;
		sLambda.sprintf("%2.0f", dLambda);
		sTheta.sprintf("%2.1f", dTheta * 180 / CV_PI);

		if(!lXLabels.contains(sTheta))
			lXLabels.append(sTheta);
		if(!lYLabels.contains(sLambda))
			lYLabels.append(sLambda);

		lResponses.append(oResponses);
	}

	QString sXTitle = tr("Kernel Orientation (in degrees)");
	QString sYTitle = tr("Kernel Wavelength (in pixels)");

	Mat oResult = ImageMan::collateMats(lResponses, Size(128, 128), oBank.wavelengths().count(), oBank.orientations().count(), true, Scalar(128), 2, Scalar(0), Scalar(255), lXLabels, lYLabels, sXTitle, sYTitle);

	namedWindow(tr("Result").toStdString(), WINDOW_AUTOSIZE);
	imshow(tr("Result").toStdString(), oResult);
	waitKey(0);

	//imwrite("c:/temp/teste.png", oResult);

	return true;
}