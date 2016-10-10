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

namespace fsdk
{
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
		 * Refreshes the text in UI elements (so translation changes
		 * can be applied).
		 */
		void refreshUI();

    private:
        
		/** Tree widget used to present the session data. */
		QTreeWidget *m_pData;

		QTreeWidgetItem *m_pRoot;

		QTreeWidgetItem *m_pVideoFolder;

		QTreeWidgetItem *m_pAnnotationsFolder;

		QTreeWidgetItem *m_pPlayerFile;

		QTreeWidgetItem *m_pGameplayFile;

    };
}

#endif // SESSIONEXPLORER_H