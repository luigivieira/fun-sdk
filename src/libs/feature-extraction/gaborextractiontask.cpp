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
#include <cmath>

using namespace cv;

// +-----------------------------------------------------------
fsdk::GaborExtractionTask::GaborExtractionTask(const QString &sVideoFile, const LandmarksData &oLandmarks):
	ExtractionTask(sVideoFile)
{
	m_oLandmarks = oLandmarks;
}

// +-----------------------------------------------------------
void fsdk::GaborExtractionTask::run()
{
	Mat oFrame;
	GaborData oData;
	QList<QPoint> lLandmarks;

	// Start the task
	start();
	
	// Process while not cancelled and there are frames
	while(!isCancelled() && !nextFrame().empty())
	{
		// Track the face in current video frame
		oFrame = frame();
		lLandmarks = m_oLandmarks.landmarks(frameIndex());

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