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

#ifndef GABORTESTAPP_H
#define GABORTESTAPP_H

#include "application.h"
#include "gaborbank.h"

namespace fsdk
{
	/**
	 * Implements a custom application to run the Gabor Test utility.
	 * It depends upon the definition of the macro 'CONSOLE' in CMakeLists.txt
	 * (check source of Application for details).
	 */
	class GaborTestApp: public Application
	{
		Q_OBJECT
	public:

		/**
		 * Class constructor.
		 * @param argc Number of arguments received from the command line.
		 * @param argv Array of char pointers with the arguments received from the command line.
		 * @param sOrgName QString with the name of the organization that manages the application.
		 * @param sOrgDomain QString with the domain of the organization in which the application exists.
		 * @param sAppName QString with the name of the application.
		 * @param sAppVersion QString with the application version.
		 * @param bUseSettings Boolean indicating if the application shall create a configuration area
		 * or not (in the Operating System's designated local, such as the Registry in Windows).
		 * The default is false (i.e. not to use settings).
		 */
		GaborTestApp(int &argc, char **argv, const QString &sOrgName, const QString &sOrgDomain, const QString &sAppName, const QString &sAppVersion, const bool bUseSettings = false);

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

		/**
		 * Parses the command line arguments.
		 * @return One of the values in the CommandLineParseResult enum indicating
		 * how the parsing of the command line proceeded.
		 */
		CommandLineParseResult parseCommandLine();

	public slots:

		/**
		 * Runs the task. The termination will be indicated
		 * by the signal finished().
		 */
		void run();

	protected:

		/**
		 * Converts angle values from degrees to radians.
		 * @param dValue Double with the angle value in degrees.
		 * @return Double with the angle value in radians.
		 */
		inline double degreesToRadians(const double dValue) const;

		/**
		 * Displays the image on a window with the given title.
		 * @param oImage OpenCV's Mat with the image to display.
		 * @param sWindowTitle QString with the title of the window used to display the image.
		 */
		void displayImage(const cv::Mat &oImage, const QString &sWindowTitle) const;

		/**
		 * Saves the image on a file.
		 * @param oImage OpenCV's Mat with the image to save.
		 * @param sFilename Path and name of the file to save the image. The formats
		 * supported are BMP, PNG, JPEG and TIFF, automatically detected from the
		 * file extension.
		 * @return Boolean indicating if the saving was successful or not.
		 */
		bool saveImage(const cv::Mat &oImage, const QString &sFilename) const;

		/**
		 * Apply the given bank of Gabor kernels to the given image (i.e. filter
		 * the image with all kernels in the bank), producing a collated image
		 * with the responses.
		 * @param oBank Reference to a GaborBank with the bank of Gabor kernels to use.
		 * @param oImage OpenCV's Mat with the image to be filtered with the bank.
		 * @return OpenCV's Mat with the image of the collated responses.
		 */
		cv::Mat applyBank(const GaborBank &oBank, const cv::Mat &oImage) const;

	private:

		/** Name of the image file to save the result. */
		QString m_sSaveImageFilename;

		/** Name of the test image to apply the filters. */
		QString m_sTestImageFilename;

		/** List of wavelenghts to create the Gabor bank. */
		QList<double> m_lLambda;

		/** List of orientations to create the Gabor bank. */
		QList<double> m_lTheta;

		/** List of standard deviations to create the Gabor bank. */
		QList<double> m_lSigma;

		/** List of phase offsets to create the Gabor bank. */
		QList<double> m_lPsi;

		/** List of kernel window sizes to create the Gabor bank. */
		QList<int> m_lWindowSize;
	};
}

#endif // GABORTESTAPP_H