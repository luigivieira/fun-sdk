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

#include "landmarksextractiontask.h"
#include "landmarksdata.h"
#include "csirofacetracker.h"

// +-----------------------------------------------------------
fsdk::LandmarksExtractionTask::LandmarksExtractionTask(QString sInputFile, float fResetQuality):
	ExtractionTask(sInputFile)
{
	m_fResetQuality = qMax(qMin(fResetQuality, 1.0f), 0.0f);	
}

// +-----------------------------------------------------------
void fsdk::LandmarksExtractionTask::run()
{
	CSIROFaceTracker oTracker;
	Mat oFrame;
	LandmarksData oData;

	// Start the task (if start fails, it will emit taskError())
	if(!start())
		return;
	
	// Process while not cancelled and there are frames
	while(!isCancelled() && !nextFrame().empty())
	{
		// Track the face in current image/video frame
		oTracker.track(frame());

		// Store the landmarks obtained in the map
		oData.add(frameIndex(), oTracker.getLandmarks(), oTracker.getQuality());

		// Reset the tracker if its quality gets lower than the configured value
		// (it makes the whole process much slower, but yields better results)
		if(oTracker.getQuality() < m_fResetQuality)
			oTracker.reset();

		// Indicate progress
		setProgress(int(float(frameIndex()) / float(frameCount()) * 100.0f));
	}

	// End the task accordingly (with cancellation or success)
	if(isCancelled())
		end(CancelRequested);
	else
		end(QVariant::fromValue(oData));
}