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

#ifndef GABORBANK_H
#define GABORBANK_H

#include "libexport.h"
#include "gaborkernel.h"
#include <QMap>
#include <QPair>

namespace fsdk
{
	/**
	 * Defines a pair of parameters for a Gabor kernel in the bank.
	 * The first parameter is the wavelength (lambda) and the second
	 * parameter is the orientation (theta).
	 */
	typedef QPair<double, double> KernelParameters;

	/**
	 * Implements a bank of Gabor filters used to represent facial expressions.
	 */
	class SHARED_LIB_EXPORT GaborBank
	{
	public:

		/**
		 * Default class constructor.
		 * Creates Gabor kernels for 6 wavelengths (3, 6, 9, 13, 17 and 21) and 8
		 * orientations (from 0 to PI in intervals of PI/8). These choices are based
		 * on the literature (refer to the thesis text for details).
		 */
		GaborBank();

		/**
		 * Class constructor that accepts the frequencies and orientations.
		 * @param lWavelengths List of double values with the wavelengths (lambda)
		 * to use when building the Gabor kernels in the bank.
		 * @param lOrientations List of double values with the orientations (theta)
		 * to use when building the Gabor kernels in the bank.
		 */
		GaborBank(const QList<double> &lWavelengths, const QList<double> &lOrientations);

		/**
		 * Copy constructor.
		 * @param oOther Constant reference to the other GaborBank from where
		 * to copy data from.
		 */
		GaborBank(const GaborBank &oOther);

		/**
		 * Assignment operator.
		 * @param oOther Constant reference to the other GaborBank from where
		 * to copy data from.
		 */
		GaborBank& operator=(const GaborBank &oOther);

		/**
		 * Gets the wavelengths used to build this bank of filters.
		 * @return QList of doubles with the wavelengths (in pixels).
		 */
		QList<double> wavelengths() const;

		/**
		 * Gets the orientations used to build this bank of filters.
		 * @return QList of doubles with the orientations (in radians).
		 */
		QList<double> orientations() const;

		/**
		 * Builds a thumbnail collation of the Gabor bank, with each kernel in the given
		 * size. The thumbnail is normalized to gray scale, so it can be used for visual
		 * inspection of the values of all kernels.
		 * @param eComp Value of the GaborKernel::KernelComponent enumeration with the
		 * component to create the thumbnail collation for. The default is GaborKernel::RealComp
		 * (i.e. thumbnails from the real components of the kernels in the bank).
		 * @param oThumbSize OpenCV Size with the size to create each kernel thumbnail.
		 * The default is Size(64, 64).
		 * @param bThumResize Boolean indicating if each thumbnail should be resized to
		 * oThumSize or not. If the thumbnails are not resized, they will be cropped and
		 * centered at oThumbSize as needed. The default is false.
		 * @return OpenCV's Mat with the image of the thumbnails of all kernels in the bank,
		 * collated together in rows and columns for each wavelength and orientation.
		 */
		cv::Mat getThumbnails(const GaborKernel::KernelComponent eComp = GaborKernel::RealComp, const cv::Size oThumbSize = cv::Size(64, 64), const bool bThumbResize = false) const;

		/**
		 * Filters the given image with the kernels in the bank and get their responses.
		 * @param oImage OpenCV's Mat with the image in which to apply the filter.
		 * @param mResponses Reference to a QMap (mappping KernelParameters to OpenCV's
		 * Mats) with the responses for each kernel in the bank.
		 */
		void filter(const cv::Mat &oImage, QMap<KernelParameters, cv::Mat> &mResponses);

		/**
		 * Filters the given image with the kernel and get the responses (that is,
		 * convolve the image with both the real and imaginary components and calculate
		 * the magnitude/energy between their responses), as well as the real and 
		 * imaginary components used when filtering.
		 * @param oImage OpenCV's Mat with the image in which to apply the filter.
		 * @param oResponses Reference to an OpenCV's Mat that will receive the filter
		 * responses.
		 * @param oReal Reference to an OpenCV's Mat that will receive the real component
		 * of the responses.
		 * @param oImaginary Reference to an OpenCV's Mat that will receive the imaginary
		 * component of the responses.
		 */
		void filter(const cv::Mat &oImage, QMap<KernelParameters, cv::Mat> &mResponses, QMap<KernelParameters, cv::Mat> &mReal, QMap<KernelParameters, cv::Mat> &mImaginary);

	private:

		/** Wavelenghts used to create the Gabor kernels in this bank. */
		QList<double> m_lWavelengths;

		/** Orientations used to create the Gabor kernels in this bank. */
		QList<double> m_lOrientations;

		/**
		 * Mapping between a pair of parameters (wavelength and orientation)
		 * and the Gabor kernel created from those parameters.
		 */
		QMap<KernelParameters, GaborKernel> m_mKernels;

	};
}

#endif // GABORBANK_H