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

#ifndef CONSOLEAPP_H
#define CONSOLEAPP_H

#include <QObject>
#include <QStringList>
#include "featureextractor.h"

namespace fsdk
{
	/**
	 * Implements the execution of the console app for the face tracker
	 */
	class ConsoleApp: public QObject
	{
		Q_OBJECT
	public:

		/**
		 * Default constructor.
		 * @param lVideoFiles QStringList with the list of video files
		 * to process (i.e. track the face landmarks).
		 * @param sOutputDir QString with the output path for saving
		 * the created CSV files.
		 * @param pParent Pointer to a QObject that will be the parent
		 * of this instance. The default is NULL (0).
		 */
		ConsoleApp(QStringList lVideoFiles, QString sOutputDir, QObject *pParent = 0);

	public slots:

		/**
		 * Runs the task. The termination will be indicated
		 * by the signal 'finished'.
		 */
		void run();

	protected slots:

		void onTaskError(QString sVideoFile, FeatureExtractor::ExtractionError eError);

		void onTaskProgress(QString sVideoFile, int iProgress);

		void onTaskFinished(QString sVideoFile);

	signals:

		/**
		 * Signals the indication that the app has terminated.
		 * @param iExitCode Integer with the exit code:
		 *		- 0 indicates success.
		 *		- 1, 2, 3, ... indicates failure in processing the
		 *        corresponding video file ('iExitCode - 1' is the
		 *        index of the failed file at the QStringList
		 *        received at the constructor).
		 */
		void finished(int iExitCode);

	private:

		/** List of video files to process. */
		QStringList m_lVideoFiles;

		/** Output directory to save the created CSV files. */
		QString m_sOutputDir;

		/** List of tasks in execution. */
		QList<FeatureExtractor*> m_lTasks;
	};
}

#endif // CONSOLEAPP_H