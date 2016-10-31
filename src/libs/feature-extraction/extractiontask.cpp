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

#include "extractiontask.h"
#include <QDebug>

// +-----------------------------------------------------------
fsdk::ExtractionTask::ExtractionTask(QString sInputFile)
{
	qRegisterMetaType<ExtractionTask::ExtractionError>("ExtractionTask::ExtractionError");
	m_sInputFile = sInputFile;
}

// +-----------------------------------------------------------
bool fsdk::ExtractionTask::start()
{
	// Try to open the file as an image
	m_oImage = cv::imread(m_sInputFile.toStdString(), CV_LOAD_IMAGE_COLOR);
	if(m_oImage.empty())
	{
		// If failed, then try to open the file as a video
		if(!m_oCap.open(m_sInputFile.toStdString()))
		{
			emit taskError(m_sInputFile, InvalidInputFile);
			return false;
		}
		else
			m_eInputFileType = VideoFile;
	}
	else
		m_eInputFileType = ImageFile;
		
	m_iCurrentFrame = -1;
	m_iProgress = 0;
	return true;
}

// +-----------------------------------------------------------
void fsdk::ExtractionTask::end(const ExtractionTask::ExtractionError &eError)
{
	m_oCap.release();
	emit taskError(m_sInputFile, eError);
}

// +-----------------------------------------------------------
void fsdk::ExtractionTask::end(const QVariant &vData)
{
	setProgress(100);
	m_oCap.release();
	emit taskFinished(m_sInputFile, vData);
}

// +-----------------------------------------------------------
void fsdk::ExtractionTask::cancel()
{
	m_oCancelRequested.testAndSetOrdered(0, 1);	
}

// +-----------------------------------------------------------
bool fsdk::ExtractionTask::isCancelled()
{
	return static_cast<int>(m_oCancelRequested) == 1;
}

// +-----------------------------------------------------------
QString fsdk::ExtractionTask::inputFile() const
{
	return m_sInputFile;
}

// +-----------------------------------------------------------
int fsdk::ExtractionTask::progress() const
{
	return m_iProgress;
}

// +-----------------------------------------------------------
void fsdk::ExtractionTask::setProgress(int iProgress)
{
	if(iProgress != m_iProgress)
	{
		m_iProgress = qMin(qMax(iProgress, 0), 100);
		emit taskProgress(m_sInputFile, iProgress);
	}
}

// +-----------------------------------------------------------
int fsdk::ExtractionTask::frameCount() const
{
	if(m_eInputFileType == VideoFile)
		return static_cast<const VideoCapture>(m_oCap).get(CV_CAP_PROP_FRAME_COUNT);
	else
		return 1;
}

// +-----------------------------------------------------------
int fsdk::ExtractionTask::frameIndex() const
{
	return m_iCurrentFrame;
}

// +-----------------------------------------------------------
Mat& fsdk::ExtractionTask::frame()
{
	return m_oCurrentFrame;
}

// +-----------------------------------------------------------
Mat& fsdk::ExtractionTask::nextFrame()
{
	if(m_eInputFileType == VideoFile)
	{
		m_oCap >> m_oCurrentFrame;
		if(!m_oCurrentFrame.empty())
			m_iCurrentFrame++;
		else
			m_iCurrentFrame = -1;
	}
	else
	{
		// Simulate reading from the image frame by frame
		// (to allow using the same interface for reading both video and image files
		// in the task classes)
		if(m_iCurrentFrame == -1)
		{
			m_oCurrentFrame = m_oImage;
			m_iCurrentFrame = 0;
		}
		else
		{
			m_oCurrentFrame = Mat();
			m_iCurrentFrame = -1;
		}
	}

	return m_oCurrentFrame;
}