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

#ifndef APPLICATION_H
#define APPLICATION_H

#include "gameplayer.h"
#include "videocapturer.h"
#include "gameplaydata.h"
#include <QApplication>
#include <QSettings>
#include <QKeySequence>
#include <QDebug>
#include <QObject>
#include <fstream>
#include <vector>
#include <QTimer>

namespace gc
{
	/**
	 * Custom Qt application class, mainly used to allow log messages to be
     * redirected to a file.
	 */
	class Application: public QApplication
	{
		Q_OBJECT
	public:
		/** Enumeration with the levels of log for the application. */
		enum LogLevel { Fatal, Critical, Warning, Info, Debug };
		Q_ENUM(LogLevel)

		/** Enumeration with the failure reason on the gameplay. */
		enum GameplayFailureReason { FailedToStart, FailedToConclude };
		Q_ENUM(GameplayFailureReason);

		/** Enumeration of the available languages for this application. */
		enum Language { EN_UK, PT_BR };
		Q_ENUM(Language)

		/**
		* Class constructor.
		* @param argc Number of arguments received from the command line.
		* @param argv Array of char pointers with the arguments received from the command line.
		*/
		Application(int& argc, char** argv);

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
		LogLevel getLogLevel();

		/**
		* Sets the maximum log level.
		* @param eLevel LogLevel with the maximum log level.
		*/
		void setLogLevel(LogLevel eLevel);

		/**
		 * Queries the language currently used by the application.
		 * @return Value of the enumeration Language with the language identifier.
		 */
		Language getLanguage();

		/**
		 * Sets the current language for the application, in order to update
		 * the translations.
		 * @param eLanguage Value of the enumeration Language with the language identifier.
		 */
		void setLanguage(Language eLanguage);

		/**
		 * Gets the settings loaded for the application.
		 * @return Instance of a QSettings with the application settings.
		 */
		QSettings* getSettings();

		/**
		 * Gets the instance of the game player, used to access the games used in the
		 * experiment.
		 * @return Instance of the GamePlayer.
		 */
		GamePlayer* gamePlayer();

		/**
		 * Gets the instance of the video capturer, used to access the videos recorded in
		 * the experiment.
		 * @return Instance of the VideoCapturer.
		 */
		VideoCapturer* videoCapturer();

		/**
		 * Queries the current subject identifier.
		 * @return Unsigned integer with the value of the current subject identifier.
		 */
		uint getSubjectID() const;

		/**
		 * Prepares the experiment to capture a new subject.
		 */
		void newSubject();

		/**
		 * Rejects the current subject and erase his/her data.
		 */
		void rejectSubject();

		/**
		 * Confirms the data of the current subject, saving his/her data.
		 */
		void confirmSubject();

		/**
		 * Starts the gameplay session in the experiment.
		 */
		void startGameplay();

		/**
		 * Stops the gameplay session in the experiment.
		 */
		void stopGameplay();

		/**
		 * Queries the gameplay video file for the current subject.
		 * @return QString with the video file name and path.
		 */
		QString getGameplayFile();

		/**
		 * Gets the configured number of samples for the gameplay review.
		 * @return Unsigned integer with the configured number of samples.
		 */
		uint getGameplayReviewSamples() const;

		/**
		 * Gets the configured time interval (in seconds) between samples in the
		 * gameplay review.
		 * @return Unsigned integer with the configured time interval in seconds.
		 */
		uint getGameplayReviewInterval() const;

		/**
		 * Gets the pointer to the gameplay data instance.
		 * @return Instance of the GameplayData used to store the captured data.
		 */
		GameplayData* getGameplayData();

		/**
		 * Sets the reference to the main window.
		 * @param pMainWindow Instance of QWidget with the pointer to the main window.
		 */
		void setMainWindow(QWidget *pMainWindow);

		/**
		 * Setups the game data based on the real duration of the video recorded.
		 * Since it is very difficult to guarantee the duration of the videos recorded,
		 * this method setups the samples in the game data for gameplay review considering
		 * as the end the real duration of the video recorded.
		 * @param iGameplayTime Unsigned integer with the real duration of the gameplay
		 * video in seconds.
		 */
		void setupGameData(uint iGameplayTime);

	protected slots:

		/**
		 * Captures the timer timeout signal (each second).
		 */
		void onTimeout();

		/**
		 * Captures the signal indicating that the gameplay started.
		 */
		void onGameplayStarted();

		/**
		 * Captures the signal indicating that the gameplay ended.
		 * @param eResult Value of the GameplayResult enumeration with the gameplay
		 * session result.
		 */
		void onGameplayEnded(GamePlayer::GameplayResult eResult);

		/**
		 * Captures the signal indicating that the video capture started.
		 */
		void onCaptureStarted();

		/**
		 * Captures the signal indicating that the video capture ended.
		 * @param eResult Value of the CaptureResult enumeration with the video
		 * capture result.
		 */
		void onCaptureEnded(VideoCapturer::CaptureResult eResult);

	signals:

		/**
		 * Signal to indicate that the application languaged changed.
		 * @param eLanguage Value of the Language enumeration indicating
		 * the language that is now active.
		 */
		void languageChanged(Application::Language eLanguage);

		/**
		 * Indicates the remaining time for the gameplay session.
		 * @param iTimeRemaining Time of gameplay remaining, in seconds.
		 */
		void gameplayTimeRemaining(uint iTimeRemaining);

		/**
		 * Indicates that that the gameplay has been successfully completed.
		 */
		void gameplayCompleted();

		/**
		 * Indicates that that the gameplay has been cancelled by the user.
		 */
		void gameplayCancelled();

		/**
		 * Indicates that the gameplay has failed to start.
		 * @param eReason Value of the enumeration GameplayFailureReason with the reason to fail.
		 */
		void gameplayFailed(GameplayFailureReason eReason);

	protected:

		/**
		 * Gets the folder where the subject data is stored.
		 * @return QString with the subject data folder.
		 */
		QString getSubjectDataFolder() const;

		/**
		 * Log and exception message handler for application events.
 		 * @param eType QtMsgType enum value with the type of the log event.
		 * @param oContext QMessageLogContext instance with information on
         * where the event happened (function, line, etc)
		 * @param sMsg QString instance with the event message.
		 */
		static void handleLogOutput(QtMsgType eType, const QMessageLogContext& oContext, const QString& sMsg);

	private:
    
		/** File stream used to log application messages. */
		std::ofstream m_oLogFile;

		/**
		 * Maximum level of the messages to log.
		 */
		LogLevel m_eLogLevel;

		/** Path where the collected data files must be stored. */
		QString m_sDataPath;

		/**
		 * String with the stylesheet to be used by the application interface
		 * (loaded from a .css file in the resource).
		 */
		QString m_sStyleSheet;

		/** Current translator installed. */
		QTranslator *m_pCurrentTranslator;

		/** Current language used by the application. */
		Language m_eCurrentLanguage;

		/** Translator used for Brazilian Portuguese. */
		QTranslator *m_pPTBRTranslator;

		/** Controls the access to the games played in the experiment. */
		GamePlayer *m_pGamePlayer;

		/** Controls the recording and access of the gameplay and player videos. */
		VideoCapturer *m_pVideoCapturer;

		/** Settings used by the application. */
		QSettings *m_pSettings;

		/** Time limit in seconds for the gameplay session. */
		uint m_iGameplayTimeLimit;

		/** Number of samples for the gameplay review. */
		uint m_iGameplayReviewSamples;

		/** Time interval in seconds for the samples in gameplay review. */
		uint m_iGameplayReviewInterval;

		/** Identifier of the current subject. */
		uint m_iSubjectID;

		/** Remaining time (in seconds) for the game session. */
		uint m_iTimeRemaining;

		/** Timer used to limit the game session. */
		QTimer m_oTimer;

		/** Indicates that a failure has already been signalled. */
		bool m_bFailureSignalled;

		/** File with the gameplay video for the current subject. */
		QString m_sGameplayFile;

		/** File with the player video of the current subject. */
		QString m_sPlayerFile;

		/** Manages the gameplay data captured for the current subject. */
		GameplayData m_oGameplayData;

		/**
		 * Indicates that the capture of gameplay is waiting for the video files
		 * to be created. This is used so the gameplay timer can be more precisely
		 * started once the videos are in fact being captured.
		 */
		bool m_bWaitingForVideoFiles;

		/** Reference to the main window. */
		QWidget *m_pMainWindow;
	};
}

#endif // APPLICATION_H
