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

using namespace cv;

// +-----------------------------------------------------------
fsdk::GaborBank::GaborBank()
{
	m_lWavelengths = QList<double>({ 3, 6, 9, 13, 17, 21 });
	for(double dTheta = 0; dTheta < CV_PI; dTheta += CV_PI / 8)
		m_lOrientations.append(dTheta);

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
fsdk::GaborBank& fsdk::GaborBank::operator=(const GaborBank &oOther)
{
	m_lWavelengths = oOther.m_lWavelengths;
	m_lOrientations = oOther.m_lOrientations;
	m_mKernels = oOther.m_mKernels;
	return *this;
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