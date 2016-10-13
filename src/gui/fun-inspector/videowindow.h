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

#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include "framewidget.h"
#include <QMdiSubWindow>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QMediaPlayer>

namespace fsdk
{
    /**
     * Window that displays the contents of a video (used to
	 * present the session videos of player's face and gameplay).
     */
    class VideoWindow : public QMdiSubWindow
    {
        Q_OBJECT

    public:
        /**
         * Class constructor.
         * @param pParent QWidget with the window parent.
         */
        VideoWindow(QWidget *pParent);

		/**
		 * Gets the size hint to be used by this widget.
		 * @return QSize with the size hint for this widget.
		 */
		QSize sizeHint() const;

		/**
		 * Gets the toggle view action for this window.
		 */
		QAction *toggleViewAction() const;

		/**
		 * Gets the goggle detached action for this window.
		 */
		QAction *toggleDetachedAction() const;

		/**
		 * Gets the menu with all actions for this window.
		 */
		QMenu *actionsMenu() const;

		/**
		 * Serializes the window state to allow saving it.
		 * @return QByteArray with the data of the window state.
		 */
		QByteArray saveState() const;

		/** 
		 * Deserializes the window state to allow restoring it.
		 * @param oData QByteArray with the data of the window state.
		 * @return Boolean indicating if the restoration was successful
		 * (true) or not (false).
		 */
		bool restoreState(const QByteArray &oData);

		/**
		 * Serializes the window geometry to allow saving it.
		 * @return QByteArray with the data of the window geometry.
		 */
		QByteArray saveGeometry() const;

		/**
		 * Deserializes the window geometry to allow restoring it.
		 * @param oData QByteArray with the data of the window geometry.
		 * @return Boolean indicating if the restoration was successful
		 * (true) or not (false).
		 */
		bool restoreGeometry(const QByteArray &oData);

		void playVideo(QString sFileName);

	public slots:

		/**
		 * Refreshes the text in UI elements (so translation changes
		 * can be applied).
		 */
		void refreshUI();

		/**
		 * Toggles the detachment of this window to the MainWindow's 
		 * MDI area.
		 * @param bDetached Boolean indicating if the window should be
		 * datached (true, case in which the window is detached from
		 * the MainWindow and hence ca be moved separatedly) or not
		 * (false, case in which the window movement is rastrained
		 * to the MainWindow's MDI area).
		 */
		void toggleDetached(bool bDetached);

		/**
		 * Toggles the visibility of this window.
		 * @param bVisible Boolean indicating if the window is visible
		 * (true) or not (false).
		 */
		void toggleVisible(bool bVisible);

	protected slots:

		/**
		 * Captures indications of changes in the media status.
		 * @param eStatus Value of the QMediaPlayer::MediaStatus with
		 * the current media status.
		 */
		void mediaStatusChanged(QMediaPlayer::MediaStatus eStatus);

		/**
		 * Captures indications of changes in the media state.
		 * @param eStatus Value of the QMediaPlayer::State with
		 * the current media state.
		 */
		void mediaStateChanged(QMediaPlayer::State eState);

		/**
		 * Captures indications of changes in the media position.
		 * @param iPosition Long integer with the position expressed
		 * in milliseconds.
		 */
		void mediaPositionChanged(qint64 iPosition);

	protected:

		/**
		 * Sets up the UI elements.
		 */
		void setupUI();

		/**
		 * Captures window events to handle them.
		 * @param pEvent Instance of QEvent with the event data.
		 */
		bool event(QEvent *pEvent);

    private:

		/** The default parent of this window (kept safe for use in detachment/attachment). */
		QWidget *m_pParent;

		/** Frame currently displayed at the window. */
		FrameWidget *m_pFrame;

		QToolBar *m_pToolbar;

		/** Toggle view action for this window. */
		QAction *m_pToggleViewAction;

		/** Toggle the detachment of this window from the MainWindow. */
		QAction *m_pToggleDetachedAction;

		/** Menu with all actions for this window. */
		QMenu *m_pActionsMenu;

		/** Media player used to play the video attached to this window. */
		QMediaPlayer *m_pMediaPlayer;

		/** Original geometry of the window when it is not maximized (so it can be properly saved and restored). */
		QRect m_oGeometry;
    };
}

#endif // VIDEOWINDOW_H