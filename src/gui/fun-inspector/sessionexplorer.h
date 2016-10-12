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

#ifndef SESSIONEXPLORER_H
#define SESSIONEXPLORER_H

#include <QDockWidget>
#include <QTreeWidget>
#include <QAction>
#include <QMenu>

namespace fsdk
{
	/**
	 * Inheritance of QTreeWidget to allow redefining its sizeHint().
	 */
	class TreeWidget : public QTreeWidget
	{
	public:
		/**
		 * Class constructor.
		 * @param pParent Instance of QWidget with the object's parent.
		 * Default is NULL.
		 */
		TreeWidget(QWidget *pParent = NULL) :
			QTreeWidget(pParent)
		{
		}

		/**
		 * Gets the size hint for this widget.
		 * @return QSize with the size hint.
		 */
		QSize sizeHint() const
		{
			return QSize(200, 400);
		}
	};

    /**
     * Dockable window that displays the loaded session content.
     */
    class SessionExplorer : public QDockWidget
    {
        Q_OBJECT

    public:
        /**
         * Class constructor.
         * @param pParent QWidget with the window parent.
         */
		SessionExplorer(QWidget *pParent = 0);

		/**
		 * Gets the menu of actions for the player file.
		 * @return Instance of a QMenu with the menu of actions.
		 */
		QMenu *playerFileMenu() const;

		/**
		 * Gets the menu of actions for the gameplay file.
		 * @return Instance of a QMenu with the menu of actions.
		 */
		QMenu *gameplayFileMenu() const;

		/**
		 * Gets the menu of actions for the facial landmarks file.
		 * @return Instance of a QMenu with the menu of actions.
		 */
		QMenu *landmarksFileMenu() const;

	public slots:

		/**
		 * Refreshes the text in UI elements (so translation changes
		 * can be applied).
		 */
		void refreshUI();

		/**
		 * Captures the signal indicating changes in the session contents and saved status.
		 * @param sSessionFileName QString with the name of the session file in disk (or empty
		 * if it is a new session).
		 * saved or not to a file in disk.
		 * @param sPlayerFileName QString with the current value of the corresponding field.
		 * @param sGameplayFileName QString with the current value of the corresponding field.
		 * @param sLandmarksFileName QString with the current value of the corresponding field.
		 */
		void sessionChanged(const QString &sSessionFileName, const QString &sPlayerFileName, const QString &sGameplayFileName, const QString &sLandmarksFileName);

	protected slots:

		/**
		 * Shows custom context menus when user right-click over specific items.
		 * @param oClickPos QPoint with the widget coordinates where the user clicked.
		 */
		void showContextMenu(const QPoint &oClickPos);

    private:
        
		/** Tree widget used to present the session data. */
		TreeWidget *m_pData;

		/** Element that holds the tree root (i.e. the node "session"). */
		QTreeWidgetItem *m_pRoot;

		/** Element that holds the videos folder. */
		QTreeWidgetItem *m_pVideosFolder;

		/** Element that holds the player video file. */
		QTreeWidgetItem *m_pPlayerFile;

		/** Element that holds the gameplay video file. */
		QTreeWidgetItem *m_pGameplayFile;

		/** Element that holds the annotations folder. */
		QTreeWidgetItem *m_pAnnotationsFolder;

		/** Element that holds the landmarks annotation file. */
		QTreeWidgetItem *m_pLandmarksFile;

		/** Menu of actions for the player file. */
		QMenu *m_pPlayerFileMenu;

		/** Action to add a player file. */
		QAction *m_pPlayerFileAddAction;

		/** Action to remove the current player file. */
		QAction *m_pPlayerFileRemoveAction;

		/** Menu of actions for the gameplay file. */
		QMenu *m_pGameplayFileMenu;

		/** Action to add a gameplay file. */
		QAction *m_pGameplayFileAddAction;

		/** Action to remove the current gameplay file. */
		QAction *m_pGameplayFileRemoveAction;

		/** Menu of actions for the landmarks file. */
		QMenu *m_pLandmarksFileMenu;

		/** Action to add a landmarks file. */
		QAction *m_pLandmarksFileAddAction;

		/** Action to remove the current landmarks file. */
		QAction *m_pLandmarksFileRemoveAction;
    };
}

#endif // SESSIONEXPLORER_H