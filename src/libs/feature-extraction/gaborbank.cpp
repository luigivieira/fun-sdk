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

#include "gaborbank.h"
#include "imageman.h"
#include <QApplication>
#include <QDebug>

using namespace cv;

// +-----------------------------------------------------------
fsdk::GaborBank::GaborBank()
{
}

// +-----------------------------------------------------------
fsdk::GaborBank::GaborBank(const QList<double> &lWavelengths, const QList<double> &lOrientations)
{
	m_lOrientations = lOrientations;
	m_lWavelengths = lWavelengths;

	foreach(double dLambda, m_lWavelengths)
	{
		foreach(double dTheta, m_lOrientations)
		{
			GaborKernel oKernel(dTheta, dLambda);
			m_mKernels[KernelParameters(dLambda, dTheta)] = oKernel;
		}
	}
}

// +-----------------------------------------------------------
fsdk::GaborBank::GaborBank(const GaborBank &oOther)
{
	m_lWavelengths = oOther.m_lWavelengths;
	m_lOrientations = oOther.m_lOrientations;
	m_mKernels = oOther.m_mKernels;
}

// +-----------------------------------------------------------
fsdk::GaborBank fsdk::GaborBank::defaultBank()
{
	QList<double> lWavelengths = QList<double>({ 3, 6, 9, 13 });
	QList<double> lOrientations;
	for(double dTheta = 0; dTheta < CV_PI; dTheta += CV_PI / 8)
		lOrientations.append(dTheta);

	GaborBank oRet;
	foreach(double dLambda, lWavelengths)
	{
		foreach(double dTheta, lOrientations)
		{
			GaborKernel oKernel(dTheta, dLambda);
			oRet.addKernel(oKernel);
		}
	}
	return oRet;
}

// +-----------------------------------------------------------
fsdk::GaborBank& fsdk::GaborBank::operator=(const GaborBank &oOther)
{
	m_lWavelengths = oOther.m_lWavelengths;
	m_lOrientations = oOther.m_lOrientations;
	m_mKernels = oOther.m_mKernels;
	return *this;
}

// +-----------------------------------------------------------
QList<double> fsdk::GaborBank::wavelengths() const
{
	return m_lWavelengths;
}


// +-----------------------------------------------------------
QList<double> fsdk::GaborBank::orientations() const
{
	return m_lOrientations;
}

// +-----------------------------------------------------------
Mat fsdk::GaborBank::getThumbnails(const GaborKernel::KernelComponent eComp, const Size oSize, const bool bResize) const
{
	QList<Mat> lThumbs;
	QStringList lXLabels, lYLabels;

	foreach(double dLambda, m_lWavelengths)
	{
		QString sLambda;
		sLambda.sprintf("%2.0f", dLambda);
		lYLabels.append(sLambda);
		foreach(double dTheta, m_lOrientations)
		{
			if(lXLabels.count() != m_lOrientations.count())
			{
				QString sTheta;
				sTheta.sprintf("%2.1f", dTheta * 180 / CV_PI);
				lXLabels.append(sTheta);
			}
			GaborKernel oKernel = m_mKernels[KernelParameters(dLambda, dTheta)];
			lThumbs.append(oKernel.getThumbnail(eComp, oSize, bResize));
		}
	}

	QString sXTitle = QApplication::translate("GaborBank", "Orientation (in degrees)");
	QString sYTitle = QApplication::translate("GaborBank", "Wavelength (in pixels)");

	return ImageMan::collateMats(lThumbs, oSize, m_lWavelengths.count(), m_lOrientations.count(), bResize, Scalar(128), 2, Scalar(255), Scalar(255), lXLabels, lYLabels, sXTitle, sYTitle);
}

// +-----------------------------------------------------------
void fsdk::GaborBank::filter(const cv::Mat &oImage, QMap<KernelParameters, cv::Mat> &mResponses) const
{
	QMap<KernelParameters, cv::Mat> mReal;
	QMap<KernelParameters, cv::Mat> mImaginary;
	filter(oImage, mResponses, mReal, mImaginary);
}

// +-----------------------------------------------------------
void fsdk::GaborBank::filter(const cv::Mat &oImage, QMap<KernelParameters, cv::Mat> &mResponses, QMap<KernelParameters, cv::Mat> &mReal, QMap<KernelParameters, cv::Mat> &mImaginary) const
{
	// Convert the image to gray scale
	Mat oGrImage;
	if(oImage.type() != CV_8UC1)
		cvtColor(oImage, oGrImage, CV_BGR2GRAY);
	else
		oGrImage = oImage;

	// Filter with all kernels
	QMap<KernelParameters, GaborKernel>::const_iterator it;
	for(it = m_mKernels.cbegin(); it != m_mKernels.cend(); ++it)
	{
		KernelParameters oParams = it.key();
		GaborKernel oKernel = it.value();

		Mat oResponses, oReal, oImaginary;
		oKernel.filter(oGrImage, oResponses, oReal, oImaginary);

		mResponses[oParams] = oResponses;
		mReal[oParams] = oReal;
		mImaginary[oParams] = oImaginary;
	}
}

// +-----------------------------------------------------------
void fsdk::GaborBank::clear()
{
	m_lOrientations.clear();
	m_lWavelengths.clear();
	m_mKernels.clear();
}

// +-----------------------------------------------------------
void fsdk::GaborBank::addKernel(const GaborKernel &oKernel)
{
	if(!m_lOrientations.contains(oKernel.theta()))
		m_lOrientations.append(oKernel.theta());
	if(!m_lWavelengths.contains(oKernel.lambda()))
	m_lWavelengths.append(oKernel.lambda());
	m_mKernels[KernelParameters(oKernel.lambda(), oKernel.theta())] = oKernel;
}

// +-----------------------------------------------------------
QList<fsdk::GaborKernel> fsdk::GaborBank::kernels() const
{
	QList<GaborKernel> oRet;
	QMap<KernelParameters, GaborKernel>::const_iterator it;
	for(it = m_mKernels.cbegin(); it != m_mKernels.cend(); ++it)
		oRet.append(it.value());
	return oRet;
}

// +-----------------------------------------------------------
Mat fsdk::GaborBank::filter(const cv::Mat &oImage) const
{
	// Convert the image to gray scale
	Mat oGrImage;
	if(oImage.type() != CV_8UC1)
		cvtColor(oImage, oGrImage, CV_BGR2GRAY);
	else
		oGrImage = oImage;

	QList<Mat> lResps;
	QStringList lXLabels, lYLabels;

	foreach(double dLambda, m_lWavelengths)
	{
		QString sLambda;
		sLambda.sprintf("%2.0f", dLambda);
		lYLabels.append(sLambda);
		foreach(double dTheta, m_lOrientations)
		{
			if(lXLabels.count() != m_lOrientations.count())
			{
				QString sTheta;
				sTheta.sprintf("%2.1f", dTheta * 180 / CV_PI);
				lXLabels.append(sTheta);
			}
			GaborKernel oKernel = m_mKernels[KernelParameters(dLambda, dTheta)];
			Mat oResp;
			oKernel.filter(oGrImage, oResp);
			lResps.append(oResp);
		}
	}

	QString sXTitle = QApplication::translate("GaborBank", "Orientation (in degrees)");
	QString sYTitle = QApplication::translate("GaborBank", "Wavelength (in pixels)");

	return ImageMan::collateMats(lResps, Size(128, 128), m_lWavelengths.count(), m_lOrientations.count(), true, Scalar(128), 2, Scalar(255), Scalar(255), lXLabels, lYLabels, sXTitle, sYTitle);
}