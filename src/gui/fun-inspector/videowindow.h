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

		/**
		 * Sets the window title.
		 * @param sTitle QString with the new window title.
		 */
		void setWindowTitle(const QString &sTitle);

	protected:

		/**
		 * Sets up the UI elements.
		 */
		void setupUI();

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
    };
}

#endif // VIDEOWINDOW_H