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
		 * Builds a thumbnail of the Gabor bank, with each kernel in the given size.
		 * The thumbnail is normalized to gray scale, so it can be useful
		 * for visual inspection of the values of all kernels.
		 * @param iThumbSize Integer with the size to create each kernel thumbnail (same
		 * value for the width and height).
		 * @param eComp Value of the GaborKernel::KernelComponent enumeration with the
		 * component to create the thumbnail for. The default is GaborKernel::RealComp
		 * (i.e. the real component).
		 * @param oBkgColor OpenCV's Scalar with the color to use for the background of the
		 * kernel images (in case the requested size is bigger than the kernel size). The
		 * default is Scalar(128), which is the gray color that represents 0 in the
		 * normalized kernel thumbnail image.
		 * @return OpenCV's Mat with the thumbnail image of the values of all kernels
		 * in the bank, for the requested component. The image is created so the thumbnails
		 * of the kernels in the bank are arranged in columns for each orientation, and rows
		 * for each wavelength.
		 */
		cv::Mat buildThumbnail(const int iThumbSize, const GaborKernel::KernelComponent eComp = GaborKernel::RealComp, const cv::Scalar oBkgColor = cv::Scalar(128)) const;

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