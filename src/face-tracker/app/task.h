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

#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QStringList>

namespace fsdk
{
	/**
	 * Implements the execution of the console application,
	 * running in the Qt's event loop.
	 */
	class Task: public QObject
	{
		Q_OBJECT
	public:

		/**
		 * Default constructor.
		 * @param lVideoFiles QStringList with the list of video files
		 * to process (i.e. track the face landmarks).
		 * @param sCSVFile QString with the CSV file to store the
		 * produced data.
		 * @param pParent Pointer to a QObject that will be the parent
		 * of this instance. The default is NULL (0).
		 */
		Task(QStringList lVideoFiles, QString sCSVFile, QObject *pParent = 0);

		/**
		 * Indicates if the task is configured to show progress information.
		 * @return Boolean indicating if the task is showing (true) or not (false)
		 * progress information.
		 */
		bool showProgress() const;

		/**
		 * Configures the task to show or not progress information.
		 * @param bShow Boolean indicating if the task shall show
		 * (true) or not (false) progress information.
		 */
		void setShowProgress(const bool bShow);

	public slots:

		/**
		 * Runs the task.
		 */
		void run();

	signals:

		/**
		 * Signal indicating that the task has concluded.
		 * @param iExitCode Integer with the exit code:
		 *		- 0 indicates success.
		 *		- 1 indicates failure in writing the CSV file.
		 *		- 2, 3, ... indicate failure in reading a video file.
		 *		  The index (in the input QStringList given at
		 *		  the constructor) of the video for which the error
		 *		  ocurred is equal to (iExitCode - 2).
		 */
		void finished(int iExitCode);

	private:

		/** List of video files to process. */
		QStringList m_lVideoFiles;

		/** CSV file to store the produced data. */
		QString m_sCSVFile;

		/** Indicates if the task should display progress information. */
		bool m_bShowProgress;
	};
}

#endif // TASK_H