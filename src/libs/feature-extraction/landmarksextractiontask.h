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

#ifndef LANDMARKSEXTRACTIONTASK_H
#define LANDMARKSEXTRACTIONTASK_H

#include "libexport.h"
#include "extractiontask.h"
#include "landmarksdata.h"
#include <QObject>
#include <QRunnable>

namespace fsdk
{
	/**
	 * Threaded-task to perform the extraction of facial landmarks.
	 */
	class SHARED_LIB_EXPORT LandmarksExtractionTask: public ExtractionTask
	{
		Q_OBJECT
	public:

		/**
		 * Class constructor.
		 * @param sVideoFile QString with the path and name of the video
		 * file to process.
		 * @param fResetQuality Float with the minimum quality that the tracker
		 * shall attempt to achieve, in range [0, 1]. During the tracking,
		 * the tracker will automatically reset (i.e. redetect the face) if
		 * the quality gets lower than this value. The default is 0.2 (20%).
		 */
		LandmarksExtractionTask(QString sVideoFile, float fResetQuality = 0.2f);

	public slots:

		/**
		 * Running method that performs the extraction. It is supposed
		 * to be automatically executed by QThreadPool, but it can be
		 * executed directly if no multithreading is intended.
		 */
		void run();

	private:

		/**
		 * Minimum ideal quality (i.e. the tracker will be reset whenever the
		 * quality gets lower than this value).
		 */
		float m_fResetQuality;
	};
}

#endif // LANDMARKSEXTRACTIONTASK_H