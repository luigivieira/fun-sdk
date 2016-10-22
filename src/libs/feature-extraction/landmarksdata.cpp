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

#include "landmarksdata.h"

// +-----------------------------------------------------------
fsdk::LandmarksData::LandmarksData(): AbstractData()
{
	qRegisterMetaType<fsdk::LandmarksData>("fsdk::LandmarksData");
}

// +-----------------------------------------------------------
fsdk::LandmarksData::LandmarksData(const LandmarksData& oOther): AbstractData(oOther)
{
	m_mLandmarks = oOther.m_mLandmarks;
	m_mQualities = oOther.m_mQualities;
}

// +-----------------------------------------------------------
fsdk::LandmarksData::~LandmarksData()
{
}

// +-----------------------------------------------------------
bool fsdk::LandmarksData::isEmpty() const
{
	return m_mLandmarks.isEmpty();
}

// +-----------------------------------------------------------
int fsdk::LandmarksData::count() const
{
	return m_mLandmarks.count();
}

// +-----------------------------------------------------------
float fsdk::LandmarksData::quality(const int iFrame) const
{
	return m_mQualities[iFrame];
}

// +-----------------------------------------------------------
void fsdk::LandmarksData::setQuality(const int iFrame, const float fValue)
{
	m_mQualities[iFrame] = qMin(qMax(fValue, 1.0f), 0.0f);
}

// +-----------------------------------------------------------
const QList<QPoint> fsdk::LandmarksData::landmarks(int iFrame) const
{
	return m_mLandmarks[iFrame];
}

// +-----------------------------------------------------------
void fsdk::LandmarksData::add(const int iFrame, const QList<QPoint> &lPoints, const float fQuality)
{
	m_mLandmarks[iFrame] = lPoints;
	m_mQualities[iFrame] = fQuality;
}

// +-----------------------------------------------------------
void fsdk::LandmarksData::remove(const int iFrame)
{
	m_mLandmarks.remove(iFrame);
	m_mQualities.remove(iFrame);
}

// +-----------------------------------------------------------
void fsdk::LandmarksData::clear()
{
	m_mLandmarks.clear();
	m_mQualities.clear();
}

// +-----------------------------------------------------------
QDebug operator<<(QDebug oDbg, const fsdk::LandmarksData &oData)
{
	if(oData.isEmpty())
		oDbg.nospace() << "LandmarksData()";
	else
	{
		oDbg.nospace() <<
		QString("LandmarksData({frame:0, quality:%1, landmarks:{(%2, %3), (more %4 ...)}}, {more %5 ...})")
		.arg(
			QString::number(oData.quality(0)),
			QString::number(oData.landmarks(0)[0].x()),
			QString::number(oData.landmarks(0)[0].y()),
			QString::number(oData.landmarks(0).count() - 1),
			QString::number(oData.count() - 1)
		);
	}
	return oDbg.maybeSpace();
}