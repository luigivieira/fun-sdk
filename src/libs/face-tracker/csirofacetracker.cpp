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

#include "csirofacetracker.h"
#include <tracker/FaceTracker.hpp>
#include <QCoreApplication>
#include <exception>

// +-----------------------------------------------------------
fsdk::CSIROFaceTracker::CSIROFaceTracker()
{
	m_pTracker = FACETRACKER::LoadFaceTracker();
	if(!m_pTracker)
	{
		QString sMsg = QCoreApplication::translate("CSIROFaceTracker", "failed to init the CSIRO face tracker");
		qCritical("%s", qPrintable(sMsg));
		throw new std::runtime_error(sMsg.toStdString());
	}

	m_pTrackerParams = FACETRACKER::LoadFaceTrackerParams();
	if(!m_pTrackerParams)
	{
		QString sMsg = QCoreApplication::translate("CSIROFaceTracker", "failed to init the CSIRO face tracker parameters");
		qCritical("%s", qPrintable(sMsg));
		delete m_pTracker;
		throw new std::runtime_error(sMsg.toStdString());
	}

	reset();
}

// +-----------------------------------------------------------
fsdk::CSIROFaceTracker::~CSIROFaceTracker()
{
	if(m_pTrackerParams)
		delete m_pTrackerParams;
	if(m_pTracker)
		delete m_pTracker;
}

// +-----------------------------------------------------------
void fsdk::CSIROFaceTracker::track(Mat &oFrame)
{
	m_lLandmarks.clear();
	int iQuality = m_pTracker->Track(oFrame, m_pTrackerParams);
	if(iQuality == FACETRACKER::FaceTracker::TRACKER_FAILED || iQuality == FACETRACKER::FaceTracker::TRACKER_FACE_OUT_OF_FRAME)
		m_fQuality = 0.0f;
	else
	{
		m_fQuality = static_cast<float>(iQuality) / 10.0f;
		FACETRACKER::PointVector vShape = m_pTracker->getShape();
		for(unsigned int i = 0; i < vShape.size(); i++)
			m_lLandmarks.append(QPoint(vShape[i].x, vShape[i].y));
	}
}

// +-----------------------------------------------------------
float fsdk::CSIROFaceTracker::getQuality() const
{
	return m_fQuality;
}

// +-----------------------------------------------------------
QList<QPoint> fsdk::CSIROFaceTracker::getLandmarks() const
{
	return m_lLandmarks;
}

// +-----------------------------------------------------------
void fsdk::CSIROFaceTracker::reset()
{
	m_pTracker->Reset();
	m_lLandmarks.clear();
	m_fQuality = 0.0f;
}