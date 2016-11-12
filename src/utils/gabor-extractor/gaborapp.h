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

#ifndef GABORAPP_H
#define GABORAPP_H

#include "application.h"
#include "gaborextractiontask.h"
#include <QMap>
#include <QList>

namespace fsdk
{
	/**
	 * Implements a custom application to run the Gabor extractor console application.
	 * It depends upon the definition of the macro 'CONSOLE' in CMakeLists.txt
	 * (check source of Application for details).
	 */
	class GaborApp: public Application
	{
		Q_OBJECT
	public:

		/**
		 * Class constructor.
		 * @param argc Number of arguments received from the command line.
		 * @param argv Array of char pointers with the arguments received from the command line.
		 * @param sOrgName QString with the name of the organization that manages the application.
		 * @param sOrgDomain QString with the domain of the organization in which the application exists.
		 * @param sAppName QString with the name of the application.
		 * @param sAppVersion QString with the application version.
		 * @param bUseSettings Boolean indicating if the application shall create a configuration area
		 * or not (in the Operating System's designated local, such as the Registry in Windows).
		 * The default is false (i.e. not to use settings).
		 */
		GaborApp(int &argc, char **argv, const QString &sOrgName, const QString &sOrgDomain, const QString &sAppName, const QString &sAppVersion, const bool bUseSettings = false);

		/**
		 * Enumeration that defines the possible outcomes of the parsing
		 * of the command line arguments.
		 */
		enum CommandLineParseResult
		{
			/** The command line was parsed correctly with all required arguments. */
			CommandLineOk,

			/** There was an error in the parsing of the command line arguments. */
			CommandLineError,

			/** The user requested the application to display its version information. */
			CommandLineVersionRequested,

			/** The user requested the application to display its help information. */
			CommandLineHelpRequested,

			/** The user requested data (like the gabor bank images) to be exported. */
			DataExportRequested
		};

		/**
		 * Parses the command line arguments.
		 * @return One of the values in the CommandLineParseResult enum indicating
		 * how the parsing of the command line proceeded.
		 */
		CommandLineParseResult parseCommandLine();

		/**
		 * Cancel the execution of the task and terminate the application.
		 */
		void cancel();

	public slots:

		/**
		 * Runs the task. The termination will be indicated
		 * by the signal finished().
		 */
		void run();

	protected slots:

		/**
		 * Captures the signal indicating an error in one of the tasks.
		 * @param sInputFile QString with the name of the image/video file of the task.
		 * @param eError Value of the enumeration FeatureExtractor::ExtractionError
		 * with the error tha happened.
		 */
		void taskError(const QString &sInputFile, const ExtractionTask::ExtractionError eError);

		/**
		 * Captures the signal indicating the progress of one of the tasks.
		 * @param sInputFile QString with the name of the image/video file of the task.
		 * @param iProgress Integer value in range [0, 100] with the progress percent.
		 */
		void taskProgress(const QString &sInputFile, int iProgress);

		/**
		 * Captures the signal indicating the conclusion of one of the tasks.
		 * @param sInputFile QString with the name of the image/video file of the task.
		 * @param vData QVariant object with the LandmarksData extracted from the
		 * input file.
		 */
		void taskFinished(const QString &sInputFile, const QVariant &vData);

	protected:

		/**
		 * Checks if the CSV file exists and, in that case, confirm with the user
		 * if she wants to have the file overwritten.
		 * @param sCSVFilename QString with the CSV file name to check.
		 * @param bAutoConfirm Reference to a boolean indicating if the confirmation
		 * to overwrite should be automatically accepted. This argument may be updated
		 * according to the user answering "yes to all".
		 * @return Boolean indicating if the file, if existing, should be overwritten
		 * (true) or if the task should be ignored to keep the existing file (false).
		 */
		bool confirmOverwrite(const QString sCSVFilename, bool &bAutoConfirm);

		/**
		 * Checks if the CSV file can be created/updated and, if it can not, confirm
		 * with the user what to do (cancel or ignore the task related to it).
		 * @param sCSVFilename QString with the CSV file name to check.
		 * @param bAutoConfirm Reference to a boolean indicating if the confirmation
		 * to ignore should be automatically accepted. This argument may be updated
		 * according to the user answering "ignore all".
		 * @param bCancel Reference to a boolean that indicates if the user has opted
		 * to cancel all tasks if one file can not be created/updated.
		 * @return Boolean indicating if the file can be created/updated (true) or 
		 * if it can not and should be ignored or all tasks cancelled (false).
		 */
		bool confirmWritable(const QString sCSVFilename, bool &bAutoIgnore, bool &bCancel);

		/**
		 * Creates a new task to extracts the Gabor responses from the given input file.
		 * The task created can then be initiated directly (by invoking task->run())
		 * or given to the QThreadPool to be executed in a different thread.
		 * @param sInputFile QString with the name of the file to process.
		 * @return Instance of GaborExtractionTask with the new task created.
		 */
		GaborExtractionTask* createTask(const QString sInputFile);

		/**
		 * Deletes the given task. The task MUST have been terminated already.
		 * To cancel a task, use the task->cancel() method.
		 * @param pTask Instance of GaborExtractionTask with the task to delete.
		 */
		void deleteTask(fsdk::GaborExtractionTask* pTask);

		/**
		 * Exports the bank of Gabor kernels used by this application, saving it
		 * as a collated image to the given file (the formats supported are BMP,
		 * PNG, JPEG and TIFF, automatically detected from the file extension).
		 * @param sFilename Path and name of the file to save the image.
		 */
		bool exportGaborBank(const QString &sFilename) const;

		/**
		 * Test the bank of Gabor filters in this application with a the given test
		 * image, displaying in a window a collated image with the responses.
		 * @param sFilename Path and name of the image file to use in the tests.
		 */
		bool testGaborBank(const QString &sFilename) const;

	private:

		/** Files to be used by the tasks (input file to read x csv file to produce). */
		QMap<QString, QString> m_mTaskFiles;

		/** List of tasks in execution. */
		QList<GaborExtractionTask*> m_lTasks;
	};
}

#endif // GABORAPP_H