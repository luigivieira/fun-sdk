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

using namespace cv;

// +-----------------------------------------------------------
fsdk::GaborBank::GaborBank()
{
	m_lWavelengths = QList<double>({ 3, 6, 9, 13, 17, 21 });
	for(double dTheta = CV_PI; dTheta > 0; dTheta -= CV_PI / 8)
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
Mat fsdk::GaborBank::buildThumbnail(const int iThumbSize, const GaborKernel::KernelComponent eComp) const
{
	Mat oRet, oThumb;
	Size oSize(iThumbSize * m_lOrientations.count(), iThumbSize * m_lWavelengths.count());

	oRet.create(oSize, CV_8UC1);

	Rect oRoi;
	int iRow = 0;
	foreach(double dLambda, m_lWavelengths)
	{
		int iCol = 0;
		foreach(double dTheta, m_lOrientations)
		{
			GaborKernel oKernel = m_mKernels[KernelParameters(dLambda, dTheta)];
			oThumb = oKernel.buildThumbnail(iThumbSize, eComp);
			oRoi = Rect(iCol * iThumbSize, iRow * iThumbSize, iThumbSize, iThumbSize);
			oThumb.copyTo(oRet(oRoi));
			iCol++;
		}
		iRow++;
	}

	return oRet;
}