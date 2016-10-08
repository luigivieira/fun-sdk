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

#ifndef APPLICATION_H
#define APPLICATION_H

#ifdef CONSOLE
	#include <QCoreApplication>
#else
	#include <QApplication>
	#include <QFile>
	#include <QTextStream>
#endif

#include <QSettings>
#include <QDebug>

namespace fsdk
{
	/**
	 * Custom application class. It can be used for both Console or GUI-based
	 * applications. The default is a GUI implementation, but if a console
	 * application is needed just define a macro named 'CONSOLE'.
	 */
	class Application :
#ifdef CONSOLE
		public QCoreApplication
#else
		public QApplication
#endif
	{
		Q_OBJECT
	public:
		/** Enumeration with the levels of log for the application. */
		enum LogLevel { Fatal, Critical, Warning, Info, Debug };
		Q_ENUM(LogLevel)

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
		Application(int &argc, char **argv, const QString &sOrgName, const QString &sOrgDomain, const QString &sAppName, const QString &sAppVersion, const bool bUseSettings = false);

		/**
		 * Class destructor.
		 */
		virtual ~Application();
        
		/**
		 * Overload of the exec method for logging startup and termination messages.
		 * @return Integer with the exit code.
		 */
		int exec();

		/**
		 * Handles the notification of messages in the application event loop.
		 * @param pReceiver Pointer to the QObject that shall receive the message.
		 * @param pEvent Pointer to the QEvent with the message information.
		 */
        bool notify(QObject* pReceiver, QEvent* pEvent);

		/**
		 * Gets the current maximum log level.
		 * @return LogLevel with the maximum log level.
		 */
		LogLevel logLevel() const;

		/**
		* Sets the maximum log level.
		* @param eLevel LogLevel with the maximum log level.
		*/
		void setLogLevel(LogLevel eLevel);

		/**
		 * Gets the settings loaded for the application, if existing.
		 * @return Instance of a QSettings with the application settings
		 * of NULL if the parameter bUseSettings was defined as false
		 * in the constructor call.
		 */
		QSettings* settings();

	protected:

		/**
		 * Custom handler for application message events.
 		 * @param eType QtMsgType enum value with the type of message event.
		 * @param oContext QMessageLogContext instance with information on
         * where the event happened (function, line, etc)
		 * @param sMessage QString instance with the event message.
		 */
		static void handleLogOutput(QtMsgType eType, const QMessageLogContext& oContext, const QString& sMessage);

	private:

		/** Maximum level of the messages to log (only used in non-console apps). */
		LogLevel m_eLogLevel;

#ifndef CONSOLE
		/** File used to log application messages. */
		QFile *m_pLogFile;

		/** Text stream used to output the application log messages (only used in non-console apps). */
		QTextStream *m_pLogStream;
#endif

		/** Settings maintained by the application. */
		QSettings *m_pSettings;
	};


}

#endif // APPLICATION_H
