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

#ifndef GABOREXTRACTIONTASK_H
#define GABOREXTRACTIONTASK_H

#include "libexport.h"
#include "extractiontask.h"
#include "landmarksdata.h"
#include "gabordata.h"

#include <opencv2\opencv.hpp>

namespace fsdk
{
	/**
	 * Threaded-task to perform the extraction of gabor responses.
	 */
	class SHARED_LIB_EXPORT GaborExtractionTask: public ExtractionTask
	{
		Q_OBJECT
	public:

		/**
		 * Class constructor.
		 * @param sVideoFile QString with the path and name of the video
		 * file to process.
		 * @param oLandmarks Object LandmarksData with the positions of the
		 * facial landmarks in the video file.
		 */
		GaborExtractionTask(const QString &sVideoFile, const LandmarksData &oLandmarks);

	public slots:

		/**
		 * Running method that performs the extraction. It is supposed
		 * to be automatically executed by QThreadPool, but it can be
		 * executed directly if no multithreading is intended.
		 */
		void run();

	protected:
		

	private:

		/** Positions of the landmarks in the video being processed. */
		LandmarksData m_oLandmarks;		
	};
}

#endif // GABOREXTRACTIONTASK_H