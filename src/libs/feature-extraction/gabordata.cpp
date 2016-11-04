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

#include "gabordata.h"
#include "csvfile.h"

// +-----------------------------------------------------------
fsdk::GaborData::GaborData()
{
	qRegisterMetaType<fsdk::GaborData>("fsdk::GaborData");
	m_iLandmarksCount = 0;
}

// +-----------------------------------------------------------
fsdk::GaborData::GaborData(const GaborData& oOther)
{
	m_mLandmarks = oOther.m_mLandmarks;
	m_iLandmarksCount = oOther.m_iLandmarksCount;
}

// +-----------------------------------------------------------
fsdk::GaborData::~GaborData()
{
}

// +-----------------------------------------------------------
bool fsdk::GaborData::isEmpty() const
{
	return m_mLandmarks.isEmpty();
}

// +-----------------------------------------------------------
int fsdk::GaborData::count() const
{
	return m_mLandmarks.count();
}

// +-----------------------------------------------------------
const QList<QPoint> fsdk::GaborData::landmarks(int iFrame) const
{
	return m_mLandmarks[iFrame];
}

// +-----------------------------------------------------------
void fsdk::GaborData::add(const int iFrame, const QList<QPoint> &lPoints, const float fQuality)
{
	m_mLandmarks[iFrame] = lPoints;
	m_iLandmarksCount = qMax(m_iLandmarksCount, lPoints.count());
}

// +-----------------------------------------------------------
void fsdk::GaborData::remove(const int iFrame)
{
	m_mLandmarks.remove(iFrame);

	m_iLandmarksCount = 0;
	QMap<int, QList<QPoint>>::const_iterator it;
	for(it = m_mLandmarks.cbegin(); it != m_mLandmarks.cend(); ++it)
		m_iLandmarksCount = qMax(m_iLandmarksCount, it.value().count());
}

// +-----------------------------------------------------------
void fsdk::GaborData::clear()
{
	m_mLandmarks.clear();
	m_iLandmarksCount = 0;
}

// +-----------------------------------------------------------
bool fsdk::GaborData::saveToCSV(const QString &sFilename) const
{
	CSVFile oData;
	QStringList lLine;

	// Add a header
	oData.header().append({ "Frame", "Quality" });

	QString sNum;
	for(int i = 0; i < m_iLandmarksCount; i++)
	{
		sNum = QString::number(i);
		oData.header().append({ QString("x%1").arg(sNum), QString("y%1").arg(sNum) });
	}

	// Add the data records
	QMap<int, QList<QPoint>>::const_iterator it;
	for(it = m_mLandmarks.cbegin(); it != m_mLandmarks.cend(); ++it)
	{
		lLine.clear();
		//lLine.append({ QString::number(it.key()), QString::number(m_mQualities[it.key()]) });
		foreach(QPoint oPoint, it.value())
			lLine.append({ QString::number(oPoint.x()), QString::number(oPoint.y()) });
		oData.addLine(lLine);
	}
		
	return oData.write(sFilename);
}

// +-----------------------------------------------------------
bool fsdk::GaborData::readFromCSV(const QString &sFilename)
{
	return false;
}

// +-----------------------------------------------------------
int fsdk::GaborData::landmarksCount() const
{
	return m_iLandmarksCount;
}

// +-----------------------------------------------------------
QDebug operator<<(QDebug oDbg, const fsdk::GaborData &oData)
{
	if(oData.isEmpty())
		oDbg.nospace() << "GaborData()";
	else
	{
		oDbg.nospace() <<
		QString("GaborData({frame:0, quality:%1, landmarks:{(%2, %3), (more %4 ...)}}, {more %5 ...})")
		.arg(
			/*QString::number(oData.quality(0)),*/
			QString::number(oData.landmarks(0)[0].x()),
			QString::number(oData.landmarks(0)[0].y()),
			QString::number(oData.landmarks(0).count() - 1),
			QString::number(oData.count() - 1)
		);
	}
	return oDbg.maybeSpace();
}