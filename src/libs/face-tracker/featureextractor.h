/*
 * Copyright (C) 2016 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
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

#ifndef FEATUREEXTRACTOR_H
#define FEATUREEXTRACTOR_H

#include "global.h"
#include <QObject>
#include <QRunnable>

namespace fsdk
{
	/**
	 * Threaded-task to perform the extraction of the required 
	 * features from a video file containing the face of a player.
	 */
	class SHARED_LIB_EXPORT FeatureExtractor: public QObject, public QRunnable
	{
		Q_OBJECT
	public:

		/**
		 * Class constructor.
		 * @param sVideoFile QString with the path and name of the video
		 * file to process.
		 * @param sLandmarksFile QString with the path and name of the
		 * CSV file to be created with the facial landmarks extracted.
		 */
		FeatureExtractor(QString sVideoFile, QString sLandmarksFile);

		/**
		 * Enumeration values indicating the different extraction errors
		 * that may happen.
		 */
		enum ExtractionError
		{
			/** Indicates that the video file could not be opened or read. */
			InvalidVideoFile,

			/** Indicates that was not possible to save the CSV file with the facial landmarks. */
			ErrorSavingLandmarks,

			/** Indicates that a cancellation was requested. */
			CancelRequested,

			/** Indicates an unknown error */
			UnknownError
		};
		Q_ENUM(ExtractionError)

		/**
		 * Running method that performs the extraction. It is supposed
		 * to be automatically executed by QThreadPool, but it can be
		 * executed directly if no multithreading is intended.
		 */
		void run();

		/**
		 * Forcefully terminate the execution of the task.
		 */
		void cancel();

	signals:

		/**
		 * Indicates if an error happened during the extraction of features.
		 * @param eError Value of the ExtractionError enumeration indicating
		 * which error happened.
		 */
		void error(QString sVideoFile, FeatureExtractor::ExtractionError eError);

		/**
		 * Indicates the progress of the feature extraction.
		 * @param iPercent Integer value in range [0, 100] indicating
		 * how much of the extraction has been concluded.
		 */
		void progress(QString sVideoFile, int iProgress);

		/**
		 * Indicates that the feature extraction has been concluded.
		 */
		void finished(QString sVideoFile);

	private:

		/** Name of the video file to process. */
		QString m_sVideoFile;

		/** Name of the CSV file to create with the facial landmarks extracted. */
		QString m_sLandmarksFile;

		/** Atomic integer to control the cancel request in a thread-safe way. */
		QAtomicInt m_oCancelRequested;

		/** Last emitted progress information (in order to avoid emitting the same value more than once). */
		int m_iLastProgress;
		
	};
}

#endif // FEATUREEXTRACTOR_H