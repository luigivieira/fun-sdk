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
	
	oParser.setApplicationDescription(tr("Allows testing the Gabor filters used by the Fun SDK project."));
	oParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	// Test image filename option
	/*oParser.addPositionalArgument("test image",
		tr("Image file to use in the test against the bank of Gabor kernels "
			"(the formats supported are BMP, PNG, JPEG and TIFF, automatically "
			"detected from the file extension)."),
		tr("[test image]")
	);*/

	QCommandLineOption oBankOpt(QStringList({ "b", "bank" }),
		tr("Displays the project's default bank of Gabor kernels to "
			"the given image file (supported formats include BMP, "
			"PNG, JPEG and TIFF, automatically detected from the "
			"file extension). This option is exclusive and cannot "
			"be used with any other with exception of -f, --file."
		)
	);
	oParser.addOption(oBankOpt);

	QCommandLineOption oLambdaOpt(QStringList({ "l", "lambda" }),
		tr("Sets the wavelength, in pixels, used to create a Gabor kernel. "
			"This is a mandatory parameter for creating a custom Gabor kernel."
		), tr("value")
	);
	oParser.addOption(oLambdaOpt);

	QCommandLineOption oThetaOpt(QStringList({ "t", "theta" }),
		tr("Sets the orientation, in degrees, used to create a Gabor kernel. "
			"This is a mandatory parameter for creating a custom Gabor kernel."
		), tr("value")
	);
	oParser.addOption(oThetaOpt);

	QCommandLineOption oSigmaOpt(QStringList({ "s", "sigma" }),
		tr("Sets the standard deviation, in radians, used to create a Gabor kernel. "
			"This parameter is not mandatory for creating a custom Gabor kernel, "
			"and will be calculated (to limit the cutoff to 0.5%) if not provided. "
		), tr("value")
	);
	oParser.addOption(oSigmaOpt);

	QCommandLineOption oPsiOpt(QStringList({ "p", "psi" }),
		tr("Sets the offset, in radians, used to create a Gabor kernel. "
			"This parameter is not mandatory for creating a custom Gabor kernel, "
			"and will be set to the default PI / 2 if not provided. "
		), tr("value")
	);
	oParser.addOption(oPsiOpt);

	QCommandLineOption oWindowSizeOpt(QStringList({ "w", "windowsize" }),
		tr("Sets the window size, in pixels, used to create a Gabor kernel. "
			"This parameter is not mandatory for creating a custom Gabor kernel, "
			"and will be calculated (according to the value of sigma) if not provided. "
			"The integer value provided is added +1 if it is even "
			"(since the window size needs to be odd) and it will "
			"be used for both the width and height of the kernel "
			"window, since this implementation supports only "
			"squared (aspect ratio = 1) kernels. "
		), tr("value")
	);
	oParser.addOption(oWindowSizeOpt);

	QCommandLineOption oFileOpt(QStringList({ "f", "file" }),
		tr("Instead of displaying the result image in a graphical window, "
			"saves the result image to the given file image file (supported "
			"formats include BMP, PNG, JPEG and TIFF, automatically detected "
			"from the file extension)."
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

	m_bBankDisplay = false;
	m_bKernelGen = false;

	// Check if exporting Gabor bank was requested
	if(oParser.isSet(oBankOpt))
		m_bBankDisplay = true;

	// Check if the Lambda parameter was provided
	double dLambda = 0;
	if(oParser.isSet(oLambdaOpt))
	{
		m_bKernelGen = true;
		QString sValue = oParser.value(oLambdaOpt);
		bool bOk;
		dLambda = sValue.toDouble(&bOk);
		if(!bOk)
		{
			qCritical().noquote() << tr("invalid lambda value %1").arg(sValue) << endl;
			oParser.showHelp();
			return CommandLineError;
		}
	}

	// Check if the Theta parameter was provided
	double dTheta = 0;
	if(oParser.isSet(oThetaOpt))
	{
		m_bKernelGen = true;
		QString sValue = oParser.value(oThetaOpt);
		bool bOk;
		dTheta = sValue.toDouble(&bOk);
		if(!bOk)
		{
			qCritical().noquote() << tr("invalid theta value %1").arg(sValue) << endl;
			oParser.showHelp();
			return CommandLineError;
		}
		dTheta = dTheta * CV_PI / 180.0;
	}

	// Check if the both theta and lambda parameters were specified for custom kernel generation
	if(m_bKernelGen)
	{
		if(!oParser.isSet(oLambdaOpt))
		{
			qCritical().noquote() << tr("the lambda value is required for created kernels.") << endl;
			oParser.showHelp();
			return CommandLineError;
		}
		if(!oParser.isSet(oThetaOpt))
		{
			qCritical().noquote() << tr("the theta value is required for created kernels.") << endl;
			oParser.showHelp();
			return CommandLineError;
		}
	}

	// Check if the Sigma parameter was provided
	double dSigma = 0;
	if(oParser.isSet(oSigmaOpt))
	{
		m_bKernelGen = true;
		QString sValue = oParser.value(oSigmaOpt);
		bool bOk;
		dSigma = sValue.toDouble(&bOk);
		if(!bOk)
		{
			qCritical().noquote() << tr("invalid sigma value %1").arg(sValue) << endl;
			oParser.showHelp();
			return CommandLineError;
		}
	}

	// Check if the Psi parameter was provided
	double dPsi = 0;
	if(oParser.isSet(oPsiOpt))
	{
		m_bKernelGen = true;
		QString sValue = oParser.value(oPsiOpt);
		bool bOk;
		dPsi = sValue.toDouble(&bOk);
		if(!bOk)
		{
			qCritical().noquote() << tr("invalid psi value %1").arg(sValue) << endl;
			oParser.showHelp();
			return CommandLineError;
		}
	}

	// Check if the WindowSize parameter was provided
	int iWindowSize = 0;
	if(oParser.isSet(oWindowSizeOpt))
	{
		m_bKernelGen = true;
		QString sValue = oParser.value(oWindowSizeOpt);
		bool bOk;
		iWindowSize = sValue.toInt(&bOk);
		if(!bOk)
		{
			qCritical().noquote() << tr("invalid window size value %1").arg(sValue) << endl;
			oParser.showHelp();
			return CommandLineError;
		}
	}

	// Check if the file option was set
	if(oParser.isSet(oFileOpt))
		m_sSaveImageFilename = oParser.value(oFileOpt);

	//***************************************
	//* Additional validations
	//***************************************

	// Check if at least one action was requested
	if(!m_bBankDisplay && !m_bKernelGen)
	{
		qCritical().noquote() << tr("no option has been defined.") << endl;
		oParser.showHelp();
		return CommandLineError;
	}

	// Check if bank display was the only request
	if(m_bBankDisplay && m_bKernelGen)
	{
		qCritical().noquote() << tr("the option -b, --bank is exclusive.") << endl;
		oParser.showHelp();
		return CommandLineError;
	}

	// Create the kernel if needed
	if(m_bKernelGen)
	{
		m_pKernel = new GaborKernel(dTheta, dLambda);
		if(oParser.isSet(oPsiOpt))
			m_pKernel->setPsi(dPsi);
		if(oParser.isSet(oSigmaOpt))
			m_pKernel->setSigma(dSigma);
		if(oParser.isSet(oWindowSizeOpt))
			m_pKernel->setWindowSize(iWindowSize);
	}
	
	// Create the bank of kernels if needed
	if(m_bBankDisplay)
		m_pBank = new GaborBank();

	return CommandLineOk;
}

// +-----------------------------------------------------------
void fsdk::GaborTestApp::run()
{
	int iRet = 0;

	if(m_bBankDisplay)
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
	}

	exit(iRet);
}

// +-----------------------------------------------------------
bool fsdk::GaborTestApp::displayGaborBank(const QString &sFilename) const
{
	if(sFilename.isEmpty())
	{
		QString sWindowName = QApplication::translate("GaborBank", "Project's Gabor Bank");
		namedWindow(sWindowName.toStdString(), WINDOW_AUTOSIZE);
		imshow(sWindowName.toStdString(), m_pBank->getThumbnails());
		waitKey(0);
		return true;
	}
	else
		return imwrite(sFilename.toStdString(), m_pBank->getThumbnails());
}

// +-----------------------------------------------------------
bool fsdk::GaborTestApp::displayKernel(const QString &sFilename) const
{
	Mat oImage = m_pKernel->getThumbnail(GaborKernel::RealComp, Size(256, 256), true);
	if(sFilename.isEmpty())
	{
		QString sWindowName = QApplication::translate("GaborBank", "Gabor Kernel");
		namedWindow(sWindowName.toStdString(), WINDOW_AUTOSIZE);
		imshow(sWindowName.toStdString(), oImage);
		waitKey(0);
		return true;
	}
	else
		return imwrite(sFilename.toStdString(), oImage);
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