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
#include "csvfile.h"
#include <QApplication>

// +-----------------------------------------------------------
fsdk::LandmarksData::LandmarksData()
{
	qRegisterMetaType<fsdk::LandmarksData>("fsdk::LandmarksData");
	m_iLandmarksCount = 0;
}

// +-----------------------------------------------------------
fsdk::LandmarksData::LandmarksData(const LandmarksData& oOther)
{
	m_mLandmarks = oOther.m_mLandmarks;
	m_mQualities = oOther.m_mQualities;
	m_iLandmarksCount = oOther.m_iLandmarksCount;
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
	m_iLandmarksCount = qMax(m_iLandmarksCount, lPoints.count());
}

// +-----------------------------------------------------------
void fsdk::LandmarksData::remove(const int iFrame)
{
	m_mLandmarks.remove(iFrame);
	m_mQualities.remove(iFrame);

	m_iLandmarksCount = 0;
	QMap<int, QList<QPoint>>::const_iterator it;
	for(it = m_mLandmarks.cbegin(); it != m_mLandmarks.cend(); ++it)
		m_iLandmarksCount = qMax(m_iLandmarksCount, it.value().count());
}

// +-----------------------------------------------------------
void fsdk::LandmarksData::clear()
{
	m_mLandmarks.clear();
	m_mQualities.clear();
	m_iLandmarksCount = 0;
}

// +-----------------------------------------------------------
bool fsdk::LandmarksData::saveToCSV(const QString &sFilename) const
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
		lLine.append({ QString::number(it.key()), QString::number(m_mQualities[it.key()]) });
		foreach(QPoint oPoint, it.value())
			lLine.append({ QString::number(oPoint.x()), QString::number(oPoint.y()) });
		oData.addLine(lLine);
	}
		
	return oData.write(sFilename);
}

// +-----------------------------------------------------------
bool fsdk::LandmarksData::readFromCSV(const QString &sFilename)
{
	CSVFile oData;
	if(!oData.read(sFilename))
	{
		qDebug().noquote() << QApplication::translate("LandmarksData", "error reading landmarks CSV file");
		return false;
	}

	QMap<int, QList<QPoint>> mLandmarks;
	QMap<int, float> mQualities;
	int iLandmarksCount = 0;

	QList<QStringList> lData = oData.lines();
	foreach(QStringList lLine, lData)
	{
		if(lLine.count() < 2)
		{
			qDebug().noquote() << QApplication::translate("LandmarksData", "format error in landmarks CSV file");
			return false;
		}

		int iFrame = lLine[0].toInt();
		float fQuality = lLine[1].toFloat();

		QList<QPoint> lPoints;
		for(int i = 2; i < lLine.count() - 1; i += 2)
		{
			int x = lLine[i].toInt();
			int y = lLine[i + 1].toInt();

			lPoints.append(QPoint(x, y));
		}

		mQualities[iFrame] = fQuality;
		mLandmarks[iFrame] = lPoints;
		iLandmarksCount = qMax(iLandmarksCount, lPoints.count());
	}		

	m_mQualities = mQualities;
	m_mLandmarks = mLandmarks;
	m_iLandmarksCount = iLandmarksCount;

	return true;
}

// +-----------------------------------------------------------
int fsdk::LandmarksData::landmarksCount() const
{
	return m_iLandmarksCount;
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