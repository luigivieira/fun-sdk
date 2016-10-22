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

#ifndef EXTRACTIONTASK_H
#define EXTRACTIONTASK_H

#include "libexport.h"
#include "abstractdata.h"
#include <QObject>
#include <QRunnable>
#include <opencv2/opencv.hpp>

using namespace cv;

namespace fsdk
{
	/**
	 * Runnable object to allow performing feature extraction tasks from video
	 * files in a multithreaded fashion. This is the abstract base class, to be
	 * inherited by specific implementations of different feature extractors.
	 */
	class SHARED_LIB_EXPORT ExtractionTask: public QObject, public QRunnable
	{
		Q_OBJECT
	public:
		/**
		 * Class constructor.
		 * @param sVideoFile QString with the path and name of the video
		 * file to process.
		 */
		ExtractionTask(QString sVideoFile);

		/**
		 * Enumeration values indicating the different extraction errors
		 * that may happen.
		 */
		enum ExtractionError
		{
			/** Indicates that the video file could not be read. */
			InvalidVideoFile,

			/** Indicates that a cancellation was requested. */
			CancelRequested,

			/** Indicates an unknown error */
			UnknownError
		};
		Q_ENUM(ExtractionError)

		/**
		 * Gets the number of frames in the video being processed.
		 */
		int frameCount() const;

		/**
		 * Gets the index of the current frame in processing.
		 * @return Integer with the index of the current frame in the video
		 * in range [0, frameCount()], or -1 if the video is not opened, no
		 * frame has been grabbed (i.e. nextFrame() has not been called yet)
		 * or if the processing reached the video's end.
		 */
		int frameIndex() const;

		/**
		 * Gets the data of the current frame in processing.
		 * @return OpenCV's Mat with the frame data. It can be an empty
		 * Mat if there is no frame available (see the documentation for 
		 * the method frameIndex()). To check if it is an empty Mat, use
		 * Mat::empty().
		 */
		Mat& frame();

		/**
		 * Grabs the next frame from the video to process.
		 * @return OpenCV's Mat with the frame data. It can be an empty
		 * Mat if there is no frame available (see the documentation for
		 * the method frameIndex()). To check if it is an empty Mat, use
		 * Mat::empty().
		 */
		Mat& nextFrame();

	public slots:

		/**
		 * Running method that performs the extraction. It is supposed
		 * to be automatically executed by QThreadPool, but it can be
		 * executed directly if no multithreading is intended.
		 * This is a pure virtual method that must be implemented by the
		 * inherited classes.
		 */
		virtual void run() = 0;

		/**
		 * Requests the early termination of the task.
		 */
		void cancel();

	signals:

		/**
		 * Indicates if an error happened during the extraction of features.
		 * @param sVideoFile QString with the name of the video file that was processed.
		 * @param eError Value of the ExtractionError enumeration indicating
		 * which error happened.
		 */
		void taskError(const QString &sVideoFile, const ExtractionTask::ExtractionError eError);

		/**
		 * Indicates the progress of the feature extraction.
		 * @param sVideoFile QString with the name of the video file that is being processed.
		 * @param iPercent Integer value in range [0, 100] indicating
		 * how much of the extraction has been concluded.
		 */
		void taskProgress(const QString &sVideoFile, const int iProgress);

		/**
		 * Indicates that the feature extraction has been concluded.
		 * @param sVideoFile QString with the name of the video file that was processed.
		 * @param oData AbstractData object with the data of the features
		 * extracted from the video file. The contents will depend on
		 * the implementation (inherited from AbstractData) used by the
		 * specific extractor (inherited from ExtractionTask).
		 */
		void taskFinished(const QString &sVideoFile, const fsdk::AbstractData &oData);

	protected:

		/**
		 * Starts up the task. This method MUST be called at the begining
		 * of the implementation of method run().
		 * @return Boolean indicating if the start up was succesful (true)
		 * or not (false). In case of unsuccess, the proper signal of error
		 * was already emitted by this method, hence the method run() can
		 * simply terminate.
		 */
		bool start();

		/**
		 * Checks if the task has been requested to terminate earlier.
		 * This method MUST be called in the loop condition of the implementation
		 * of the method run().
		 * @return Boolean indicating if the cancellation was requested (true) or
		 * not (false). In case of cancellation, the proper signal of cancelation
		 * was already emitted by this method, hence the method run() can
		 * simply terminate.
		 */
		bool isCancelled();

		/**
		 * Ends the task with an error. This method MUST be called at the
		 * implementation of method run() whenever an error ocurred. The
		 * proper signal of cancelation will be emitted by this method,
		 * hence the method run() can simply terminate after calling this
		 * method.
		 * @param eError Value of the ExtractionError enumeration indicating
		 * which error happened.
		 */
		void end(const ExtractionTask::ExtractionError &eError);

		/**
		 * Ends the task with success, providing the data with the features
		 * extracted. This method MUST be called at the implementation of run()
		 * when it concludes. The proper signal of conclusion will be emitted
		 * by this method, hence the method run() can simply terminate after
		 * calling this method.
		 * @param oData AbstractData object with the data of the features
		 * extracted from the video file. The contents will depend on
		 * the implementation (inherited from AbstractData) used by the
		 * specific extractor (inherited from ExtractionTask).
		 */
		void end(const fsdk::AbstractData &oData);

		/**
		 * Queries the video file assigned to the task.
		 * @return QString with the name of the video file.
		 */
		QString videoFile() const;

		/**
		 * Gets the progress of the task.
		 * @return Integer with the current progress in range [0, 100].
		 */
		int progress() const;

		/**
		 * Sets the progress of the task.
		 * @param iProgress Integer with the current progress in range [0, 100].
		 */
		void setProgress(int iProgress);

	private:

		/** OpenCV's object used to read the video files. */
		VideoCapture m_oCap;

		/** Name of the video file to process. */
		QString m_sVideoFile;

		/** Atomic integer to control the cancel request in a thread-safe way. */
		QAtomicInt m_oCancelRequested;

		/** Current progress in range [0, 100]. */
		int m_iProgress;

		/** Data of the current frame being processed in the video. */
		Mat m_oCurrentFrame;

		/** Index of the current frame being processed in the video. */
		int m_iCurrentFrame;
	};
}

#endif // EXTRACTIONTASK_H