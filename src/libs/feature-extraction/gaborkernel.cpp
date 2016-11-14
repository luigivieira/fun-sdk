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

#include "gaborkernel.h"

using namespace cv;

// +-----------------------------------------------------------
fsdk::GaborKernel::GaborKernel()
{
	m_dTheta = 0;
	m_dLambda = 3;
	m_dPsi = CV_PI / 2;
	m_dSigma = 0;
	m_iWindowSize = 3;
}

// +-----------------------------------------------------------
fsdk::GaborKernel::GaborKernel(const int iWindowSize, const double dTheta, const double dLambda, const double dSigma, const double dPsi)
{
	m_dTheta = std::min(std::max(dTheta, 0.0), CV_PI) * -1; // Theta is only flipped to visually simulate the "natural" orientation of angles
	m_dLambda = std::max(dLambda, 3.0);
	m_dPsi = std::min(std::max(dPsi, 0.0), CV_PI);
	m_dSigma = std::max(dSigma, 0.0);
	m_iWindowSize = std::max(iWindowSize, 3);
	if(m_iWindowSize % 2 == 0)
		m_iWindowSize++;

	rebuildKernel();
}

// +-----------------------------------------------------------
fsdk::GaborKernel::GaborKernel(const double dTheta, const double dLambda, const double dPsi)
{
	m_dTheta = std::min(std::max(dTheta, 0.0), CV_PI) * -1; // Theta is only flipped to visually simulate the "natural" orientation of angles
	m_dLambda = std::max(dLambda, 3.0);
	m_dPsi = std::min(std::max(dPsi, 0.0), CV_PI);
	m_dSigma = 0.56 * m_dLambda;
	m_iWindowSize = 1 + 2 * std::ceil(std::sqrt(-2 * std::pow(m_dSigma, 2) * std::log(0.005))); // Kernel size is based on sigma (to limit the cutoff to 0.5%)

	rebuildKernel();
}

// +-----------------------------------------------------------
fsdk::GaborKernel::GaborKernel(const GaborKernel &oOther)
{
	m_dTheta = oOther.m_dTheta;
	m_dLambda = oOther.m_dLambda;
	m_dPsi = oOther.m_dPsi;
	m_dSigma = oOther.m_dSigma;
	m_iWindowSize = oOther.m_iWindowSize;

	m_oRealComp = oOther.m_oRealComp.clone();
	m_oImaginaryComp = oOther.m_oImaginaryComp.clone();
}

// +-----------------------------------------------------------
fsdk::GaborKernel& fsdk::GaborKernel::operator=(const GaborKernel &oOther)
{
	m_dTheta = oOther.m_dTheta;
	m_dLambda = oOther.m_dLambda;
	m_dPsi = oOther.m_dPsi;
	m_dSigma = oOther.m_dSigma;
	m_iWindowSize = oOther.m_iWindowSize;

	m_oRealComp = oOther.m_oRealComp.clone();
	m_oImaginaryComp = oOther.m_oImaginaryComp.clone();

	return *this;
}

// +-----------------------------------------------------------
void fsdk::GaborKernel::rebuildKernel()
{
	// Then, calculate the Gabor kernel for both the real and imaginary parts
	int x, y;
	int iHalfSize = (m_iWindowSize - 1) / 2; // Half the even size, so kernel's max value is "centered" at (0, 0)
	double dThetaX, dThetaY, dGauss, dRealSignal, dImagSignal;

	m_oRealComp.create(m_iWindowSize, m_iWindowSize, CV_32F);
	m_oImaginaryComp.create(m_iWindowSize, m_iWindowSize, CV_32F);

	for(y = -iHalfSize; y <= iHalfSize; y++)
	{
		for(x = -iHalfSize; x <= iHalfSize; x++)
		{
			// Direction of the sinusoidal carrier component
			dThetaX = x * std::cos(m_dTheta) + y * std::sin(m_dTheta);
			dThetaY = -x * std::sin(m_dTheta) + y * std::cos(m_dTheta);

			// Value of the Gaussian envelope at (x, y)
			dGauss = exp(-((std::pow(dThetaX, 2) + std::pow(dThetaY, 2)) / (2 * std::pow(m_dSigma, 2))));

			// Real value of the sinusoidal carrier at (x, y)
			dRealSignal = (std::cos(2 * CV_PI * dThetaX / m_dLambda + m_dPsi));

			// Imaginary value of the sinusoidal carrier at (x, y)
			dImagSignal = (std::sin(2 * CV_PI * dThetaX / m_dLambda + m_dPsi));

			// Multiply the envelope and the carrier to build the kernel components
			m_oRealComp.at<float>(y + iHalfSize, x + iHalfSize) = (float)(dGauss * dRealSignal);
			m_oImaginaryComp.at<float>(y + iHalfSize, x + iHalfSize) = (float)(dGauss * dImagSignal);
		}
	}	
}

// +-----------------------------------------------------------
Mat fsdk::GaborKernel::data(const KernelComponent eComp)
{
	if(eComp == RealComp)
		return m_oRealComp;
	else
		return m_oImaginaryComp;
}

// +-----------------------------------------------------------
Mat fsdk::GaborKernel::getThumbnail(const KernelComponent eComp, const Size oSize, const bool bResize) const
{
	// Get a clonned image of the kernel
	Mat oPart = (eComp == RealComp ? m_oRealComp : m_oImaginaryComp).clone();

	// "Normalize" the kernel image so -1 becomes 0 (black), 0 becomes 128 (gray)
	// and 1 becomes 255 (white) - needed to make the kernel image more visible
	oPart += 1.0;
	oPart *= 128.0;
	oPart.convertTo(oPart, CV_8UC1);

	// If the kernel has the same size as it was requested, just return it
	if(oSize.width == oPart.size().width && oSize.height == oPart.size().height)
		return oPart;

	// If not the same size but the resize has been request, just do it! :)
	if(bResize)
	{
		resize(oPart, oPart, oSize);
		return oPart;
	}

	// Otherwise, build the return accordingly to the requested size, cropping the image
	// as needed
	Mat oRet;
	oRet.create(oSize, CV_8UC1);
	oRet = Scalar(128); // The value 0 after the normalization above is the background color
		
	// Crop the thumbnail horizontally and vertically if needed
	Rect oClip(0, 0, oPart.size().width, oPart.size().height);
	if(oPart.size().width > oSize.width)
	{
		int iDiff = oPart.size().width - oSize.width;
		oClip.x += iDiff / 2;
		oClip.width -= iDiff;
	}
	if(oPart.size().height > oSize.height)
	{
		int iDiff = oPart.size().height - oSize.height;
		oClip.y += iDiff / 2;
		oClip.height -= iDiff;
	}
	if(oClip.x != 0 || oClip.y != 0)
		oPart = oPart(oClip);

	// Copy the thumbnail image to the center of the return image
	Rect oTgtArea = Rect(oSize.width / 2 - oPart.size().width / 2, oSize.height / 2 - oPart.size().height / 2, oPart.size().width, oPart.size().height);
	oPart.copyTo(oRet(oTgtArea));

	return oRet;
}

// +-----------------------------------------------------------
double fsdk::GaborKernel::theta() const
{
	return std::abs(m_dTheta);
}

// +-----------------------------------------------------------
void fsdk::GaborKernel::setTheta(const double dValue)
{
	m_dTheta = std::min(std::max(dValue, 0.0), CV_PI) * -1;
	rebuildKernel();
}

// +-----------------------------------------------------------
double fsdk::GaborKernel::lambda() const
{
	return m_dLambda;
}

// +-----------------------------------------------------------
void fsdk::GaborKernel::setLambda(const double dValue)
{
	m_dLambda = std::max(dValue, 3.0);
	rebuildKernel();
}

// +-----------------------------------------------------------
double fsdk::GaborKernel::psi() const
{
	return m_dPsi;
}

// +-----------------------------------------------------------
void fsdk::GaborKernel::setPsi(const double dValue)
{
	m_dPsi = std::min(std::max(dValue, 0.0), CV_PI);
	rebuildKernel();
}

// +-----------------------------------------------------------
double fsdk::GaborKernel::sigma() const
{
	return m_dSigma;
}

// +-----------------------------------------------------------
void fsdk::GaborKernel::setSigma(const double dValue)
{
	m_dSigma = std::max(dValue, 0.0);
	rebuildKernel();
}

// +-----------------------------------------------------------
int fsdk::GaborKernel::windowSize() const
{
	return m_iWindowSize;
}

// +-----------------------------------------------------------
void fsdk::GaborKernel::setWindowSize(int iValue)
{
	m_iWindowSize = std::max(iValue, 3);
	if(m_iWindowSize % 2 == 0)
		m_iWindowSize++;
	rebuildKernel();
}

// +-----------------------------------------------------------
double fsdk::GaborKernel::bandWidth() const
{
	double dRatio = (m_dSigma / m_dLambda) * CV_PI;
	double dRoot = std::sqrt(std::log(2) / 2);
	return std::log((dRatio + dRoot) / (dRatio - dRoot)) / std::log(2);
}

// +-----------------------------------------------------------
void fsdk::GaborKernel::filter(const Mat &oImage, Mat &oResponses)
{
	Mat oReal, oImaginary;
	filter(oImage, oResponses, oReal, oImaginary);
}

// +-----------------------------------------------------------
void fsdk::GaborKernel::filter(const Mat &oImage, Mat &oResponses, Mat &oReal, Mat &oImaginary)
{
	// Convert the image to gray scale
	Mat oGrImage;
	cvtColor(oImage, oGrImage, CV_BGR2GRAY);

	// Convolve the image with the two components
	filter2D(oGrImage, oReal, CV_32F, m_oRealComp);
	filter2D(oGrImage, oImaginary, CV_32F, m_oImaginaryComp);

	// Calculate the response (the magnitude/energy)
	magnitude(oReal, oImaginary, oResponses);

	/*double dReal, dImag, dMag;
	oResponses.create(oImage.size(), CV_32F);
	for(int iRow = 0; iRow < oImage.rows; iRow++)
	{
		for(int iCol = 0; iCol < oImage.cols; iCol++)
		{
			dReal = oReal.at<float>(iRow, iCol);
			dImag = oImaginary.at<float>(iRow, iCol);
			dMag = std::sqrt(std::pow(dReal, 2) + std::pow(dImag, 2));
			oResponses.at<float>(iRow, iCol) = dMag;
		}
	}*/
}