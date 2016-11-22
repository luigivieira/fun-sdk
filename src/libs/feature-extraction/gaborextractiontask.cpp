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

#include "gaborextractiontask.h"
#include "landmarksdata.h"
#include <cmath>
#include <QRect>
#include <QVector2D>
#include "csirofacetracker.h"

using namespace cv;

// Indexes of the landmarks at the eye corners
#define LEFT_EYE_INNER_CORNER 39
#define RIGHT_EYE_INNER_CORNER 42

// +-----------------------------------------------------------
fsdk::GaborExtractionTask::GaborExtractionTask(const QString &sVideoFile, const QString &sLandmarksFile):
	ExtractionTask(sVideoFile)
{
	m_sLandmarksFile = sLandmarksFile;
	m_oBank = GaborBank::defaultBank();
}

// +-----------------------------------------------------------
void fsdk::GaborExtractionTask::run()
{
	Mat oFrame;
	GaborData oData;
	QList<QPoint> lLandmarks;

	// Try to read the CSV with the landmarks
	LandmarksData oLandmarks;
	if(!oLandmarks.readFromCSV(m_sLandmarksFile))
	{
		end(InvalidInputParameters);
		return;
	}

	// Start the task
	start();
	
	// Process while not cancelled and there are frames
	while(!isCancelled() && !nextFrame().empty())
	{
		// Get current frame and its landmarks
		oFrame = frame();
		lLandmarks = oLandmarks.landmarks(frameIndex());

		// Ignore frames where there is no landmarks (i.e. the tracking quality was 0)
		if(lLandmarks.count() == 0)
		{
			setProgress(int(float(frameIndex()) / float(frameCount()) * 100.0f));
			continue;
		}

		// Crop the face region and normalize its image (so the distance
		// between eyes is 50 pixels)
		oFrame = cropAndNormalize(oFrame, lLandmarks);

		// Filter the frame with the bank of Gabor kernels
		QList<Mat> lResponses;
		m_oBank.filter(oFrame, lResponses);

		// Store the landmarks obtained in the map
		//oData.add(frameIndex(), oTracker.getLandmarks(), oTracker.getQuality());

		// Indicate progress
		setProgress(int(float(frameIndex()) / float(frameCount()) * 100.0f));
	}

	// End the task accordingly (with cancellation or success)
	if(isCancelled())
		end(CancelRequested);
	else
		end(QVariant::fromValue(oData));
}

// +-----------------------------------------------------------
Mat fsdk::GaborExtractionTask::cropAndNormalize(const Mat &oImage, const QList<QPoint> &lLandmarks) const
{
	// Get the face region: the region that encloses all landmarks
	int iMinX = oImage.cols;
	int iMinY = oImage.rows;
	int iMaxX = 0;
	int iMaxY = 0;
	for(uint i = 0; i < CSIROFaceTracker::landmarksCount(); i++)
	{
		QPoint oPoint = lLandmarks[i];
		if(oPoint.x() < iMinX)
			iMinX = oPoint.x();
		if(oPoint.y() < iMinY)
			iMinY = oPoint.y();
		if(oPoint.x() > iMaxX)
			iMaxX = oPoint.x();
		if(oPoint.y() > iMaxY)
			iMaxY = oPoint.y();
	}

	// Give a margin of 10 pixels around the face region
	iMinX -= 10; if(iMinX < 0) iMinX = 0;
	iMinY -= 10; if(iMinY < 0) iMinY = 0;
	iMaxX += 10; if(iMaxX >= oImage.cols) iMaxX = oImage.cols - 1;
	iMaxY += 10; if(iMaxY >= oImage.rows) iMaxY = oImage.rows - 1;

	// Crop the image to keep only the face region
	Mat oRet = oImage(Rect(iMinX, iMinY, iMaxX - iMinX, iMaxY - iMinY));

	// Calculate the distance between the eyes
	QPoint oLeftEye(lLandmarks[LEFT_EYE_INNER_CORNER]);
	QPoint oRightEye(lLandmarks[RIGHT_EYE_INNER_CORNER]);
	float fDistance = std::ceil(QVector2D(oRightEye - oLeftEye).length());

	// Scale the image so the distance between the eyes is close to 50 pixels
	float fScale = 50.0 / fDistance;
	Size oSize = oRet.size();
	oSize.width *= fScale;
	oSize.height *= fScale;
	resize(oRet, oRet, oSize);

	return oRet;
}