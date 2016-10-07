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

#include "featureextractor.h"
#include "csirofacetracker.h"
#include <opencv2/opencv.hpp>
#include "csvfile.h"
#include <QDebug>

// +-----------------------------------------------------------
fsdk::FeatureExtractor::FeatureExtractor(QString sVideoFile, QString sLandmarksFile)
{
	qRegisterMetaType<FeatureExtractor::ExtractionError>("FeatureExtractor::ExtractionError");
	m_sVideoFile = sVideoFile;
	m_sLandmarksFile = sLandmarksFile;
}

// +-----------------------------------------------------------
void fsdk::FeatureExtractor::cancel()
{
	m_oCancelRequested.testAndSetOrdered(0, 1);
}

// +-----------------------------------------------------------
void fsdk::FeatureExtractor::run()
{
	qInfo() << tr("starting feature extraction from file %1...").arg(m_sVideoFile);

	// Open the video for reading
	VideoCapture oCap;
	if(!oCap.open(m_sVideoFile.toStdString()))
	{
		qCritical() << tr("error reading file %1").arg(m_sVideoFile);
		emit error(m_sVideoFile, InvalidVideoFile);
		return;
	}

	// Prepare the CSV for the facial landmarks
	CSVFile oLandmarks(m_sLandmarksFile, CSIROFaceTracker::landmarksCount() * 2 + 2);
	QStringList lHeader;
	lHeader.append("Frame Number");
	lHeader.append("Tracking Quality");
	for(uint i = 0; i < CSIROFaceTracker::landmarksCount(); i++)
	{
		lHeader.append(QString("x%1").arg(i));
		lHeader.append(QString("y%1").arg(i));
	}
	oLandmarks.setHeader(lHeader);

	// Ok: process!
	CSIROFaceTracker oTracker;
	Mat oFrame;

	emit progress(m_sVideoFile, 0);
	m_iLastProgress = 0;

	int iFrameCount = oCap.get(CV_CAP_PROP_FRAME_COUNT);
	for(int iFrame = 1; iFrame <= iFrameCount; iFrame++)
	{
		// Track the face in current video frame
		oCap >> oFrame;
		oTracker.track(oFrame);

		// Store the landmarks in CSV
		QStringList lRow = { QString::number(iFrame), QString::number(oTracker.getQuality(), 'f', 2) };
		QList<QPoint> vLandmarks = oTracker.getLandmarks();
		if(vLandmarks.count() > 0)
		{
			foreach(QPoint oLandmark, vLandmarks)
			{
				lRow.append(QString::number(oLandmark.x()));
				lRow.append(QString::number(oLandmark.y()));
			}
		}
		else
		{
			for(uint i = 0; i < CSIROFaceTracker::landmarksCount(); i++)
			{
				lRow.append("");
				lRow.append("");
			}
		}
		if(!oLandmarks.addRow(lRow))
		{
			emit error(m_sVideoFile, ErrorSavingLandmarks);
			return;
		}

		// Signal progress or cancel if requested
		if(!static_cast<int>(m_oCancelRequested))
		{
			int iProgress = int(float(iFrame) / float(iFrameCount) * 100.0f);
			if(iProgress != m_iLastProgress)
			{
				m_iLastProgress = iProgress;
				emit progress(m_sVideoFile, iProgress);
			}
		}
		else
		{
			emit error(m_sVideoFile, CancelRequested);
			return;
		}
	}

	// Finally save the data produced
	if(!oLandmarks.save())
	{
		switch(oLandmarks.error())
		{
		case QFileDevice::NoError:
		case QFileDevice::UnspecifiedError:
			emit error(m_sVideoFile, UnknownError);
			break;

		default:
			emit error(m_sVideoFile, ErrorSavingLandmarks);
		}
		return;
	}

	// Indicate successful conclusion
	emit finished(m_sVideoFile);
}