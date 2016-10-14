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
 
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "videowindow.h"
#include "sessionexplorer.h"
#include "session.h"
#include "mediasynchronizer.h"
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QToolBar>

namespace fsdk
{
    /**
     * Main window class.
     */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        /**
         * Class constructor.
         * @param pParent QWidget with the parent window.
         */
        explicit MainWindow(QWidget *pParent = 0);

		/**
		 * Get the last path used for open/save operations.
		 * @return QString with the last path used for open/save operations.
		 */
		QString lastPathUsed() const;

		/**
		 * Sets the last path used for open/save operations.
		 * @param sPath QString with the last path used for open/save operations.
		 */
		void setLastPathUsed(const QString &sPath);

	public slots:

		/**
		 * Refreshes the text in UI elements (so translation changes
		 * can be applied).
		 */
		void refreshUI();

	protected:

		/**
		 * Event filter that captures events from subwindows in order to
		 * detect their activation.
		 * @param pObject Instance of QObject with the object.
		 * @param pEvent Instance of QEvent with the event received.
		 * @return Boolean indicating if the event was blocked here (true)
		 * or not (false).
		 */
		bool eventFilter(QObject *pObject, QEvent *pEvent);

		/**
		 * Captures the event of window show.
		 * @param pEvent QShowEvent instance with the event data.
		 */
		void showEvent(QShowEvent *pEvent);

		/**
		 * Captures the event of window close.
		 * @param pEvent QCloseEvent instance with the event data.
		 */
		void closeEvent(QCloseEvent *pEvent);

		/**
		 * Sets up the UI elements.
		 */
		void setupUI();

		/**
		 * Saves the z-order of all sub windows to a semicolon-separated
		 * string.
		 * @return QString with the object names of all sub windows
		 * in the current z-order, separated by a ';'.
		 */
		QString saveZOrder() const;

		/**
		 * Restores the z-order of all sub windows from a semicolon-separated
		 * string.
		 * @param sData QString with the object names of all sub windows
		 * in the intended z-order, separated by a ';'.
		 */
		bool restoreZOrder(const QString &sData);

	protected slots:

		/**
		 * Unloads the application, saving its settings.
		 */
		void unloadApp() const;

		/**
		 * Loads the application settings.
		 */
		void loadApp();

		/**
		 * Creates a new and empty session.
		 */
		void newSession();

		/**
		 * Opens a session file selected by the user.
		 * @return Boolean indicating if the loading was successful (true)
		 * or not (false).
		 */
		bool openSession();

		/**
		 * Saves the current session file.
		 * @return Boolean indicating if the saving was successful (true)
		 * or not (false).
		 */
		bool saveSession();

		/**
		 * Saves the current session file with a different file name
		 * selected by the user.
		 * @return Boolean indicating if the saving was successful (true)
		 * or not (false).
		 */
		bool saveSessionAs();

		/**
		 * Quits the application.
		 */
		void quit();

		/**
		 * Shows help for the application.
		 */
		void help();

		/**
		* Shows information about the application.
		*/
		void about();

		/**
		 * Tiles the attached subwindows horizontally.
		 */
		void tileHorizontally();

		/**
		 * Tiles the attached subwindows vertically.
		 */
		void tileVertically();

    private:

		/** Synchronizer for the playback of session videos. */
		MediaSynchronizer *m_pMediaSync;

		/** Allows the access to the session data. */
		Session *m_pSessionData;

		/** Window that displays the session explorer. */
		SessionExplorer *m_pSessionExplorer;

		//-------------------------------
		// Sub windows
		//-------------------------------

		/** Z-ordered vector of all sub windows in the application. */
		QVector<QWidget*> m_vSubWindows;

		/** Window that displays the player's face video. */
		VideoWindow *m_pPlayerWindow;

		/** Window that displays the gameplay video. */
		VideoWindow *m_pGameplayWindow;

		//-------------------------------
		// "Session" menu/toolbar
		//-------------------------------

		/** Main menu item called "Session". */
		QMenu *m_pSessionMenu;

		/** Toolbar for the "Session" menu. */
		QToolBar *m_pSessionToolbar;

		/** Action called "New": creates a new session. */
		QAction *m_pNewAction;

		/** Action called "Open": opens a session file. */
		QAction *m_pOpenAction;

		/** Action called "Save": saves the current session file. */
		QAction *m_pSaveAction;

		/** Action called "Save As": saves the current session file with a different name. */
		QAction *m_pSaveAsAction;

		/** Session menu's submenu called "Files" */
		QMenu *m_pSessionFilesMenu;

		/** Action called "Exit": terminates the application. */
		QAction *m_pExitAction;

		//-------------------------------
		// "Playback" menu/toolbar
		//-------------------------------

		/** Main menu item called "Playback". */
		QMenu *m_pPlaybackMenu;

		/** Toolbar for the "Playback" menu. */
		QToolBar *m_pPlaybackToolbar;

		/** Action called "Play/Pause": plays/pauses the session videos. */
		QAction *m_pTogglePlayPauseAction;

		/** Action called "Stop": stops the session videos. */
		QAction *m_pStopAction;

		//-------------------------------
		// "View" menu/toolbar
		//-------------------------------

		/** Main menu item called "View". */
		QMenu *m_pViewMenu;

		/** View menu's submenu called "Windows". */
		QMenu *m_pViewWindowsMenu;

		/** Action called "Tile Horizontally": tiles the subwindows in a horizontal fashion. */
		QAction *m_pTileHorizontalAction;

		/** Action called "Tile Vertically": tiles the subwindows in a vertical fashion. */
		QAction *m_pTileVerticalAction;

		/** View menu's submenu called "Toolbars". */
		QMenu *m_pViewToolbarsMenu;

		//-------------------------------
		// "Help" menu/toolbar
		//-------------------------------

		/** Main menu item called "Help". */
		QMenu *m_pHelpMenu;

		/** Action called "Help": shows online help. */
		QAction *m_pHelpAction;

		/** Action called "About": shows application information. */
		QAction *m_pAboutAction;

		/** Last used path for open/save operations. */
		QString m_sLastPathUsed;
    };
};

#endif // MAINWINDOW_H
