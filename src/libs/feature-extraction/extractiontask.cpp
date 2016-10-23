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
fsdk::ExtractionTask::ExtractionTask(QString sVideoFile)
{
	qRegisterMetaType<ExtractionTask::ExtractionError>("ExtractionTask::ExtractionError");
	m_sVideoFile = sVideoFile;
}

// +-----------------------------------------------------------
bool fsdk::ExtractionTask::start()
{
	if(!m_oCap.open(m_sVideoFile.toStdString()))
	{
		emit taskError(m_sVideoFile, InvalidVideoFile);
		return false;
	}
	m_iCurrentFrame = -1;
	setProgress(0);
	return true;
}

// +-----------------------------------------------------------
void fsdk::ExtractionTask::end(const ExtractionTask::ExtractionError &eError)
{
	m_oCap.release();
	emit taskError(m_sVideoFile, eError);
}

// +-----------------------------------------------------------
void fsdk::ExtractionTask::end(const QVariant &vData)
{
	m_oCap.release();
	emit taskFinished(m_sVideoFile, vData);
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
QString fsdk::ExtractionTask::videoFile() const
{
	return m_sVideoFile;
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
		emit taskProgress(m_sVideoFile, iProgress);
	}
}

// +-----------------------------------------------------------
int fsdk::ExtractionTask::frameCount() const
{
	return static_cast<const VideoCapture>(m_oCap).get(CV_CAP_PROP_FRAME_COUNT);
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
	m_oCap >> m_oCurrentFrame;
	if(!m_oCurrentFrame.empty())
		m_iCurrentFrame++;
	else
		m_iCurrentFrame = -1;
	return m_oCurrentFrame;
}