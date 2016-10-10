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

#include "sessionexplorer.h"
#include <QHeaderView>

// +-----------------------------------------------------------
fsdk::SessionExplorer::SessionExplorer(QWidget *pParent) :
	QDockWidget("", pParent)
{
	m_pData = new QTreeWidget(this);
	setWidget(m_pData);
	
	m_pData->setColumnCount(2);
	m_pData->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_pData->header()->close();
	
	m_pRoot = new QTreeWidgetItem();
	m_pRoot->setIcon(0, QIcon(":/icons/session-folder.png"));
	m_pData->addTopLevelItem(m_pRoot);

	m_pVideoFolder = new QTreeWidgetItem();
	m_pVideoFolder->setIcon(0, QIcon(":/icons/videos-folder.png"));
	m_pRoot->addChild(m_pVideoFolder);

	m_pAnnotationsFolder = new QTreeWidgetItem();
	m_pAnnotationsFolder->setIcon(0, QIcon(":/icons/annotations-folder.png"));
	m_pRoot->addChild(m_pAnnotationsFolder);

	m_pPlayerFile = new QTreeWidgetItem();
	m_pPlayerFile->setIcon(0, QIcon(":/icons/player-file.png"));
	m_pVideoFolder->addChild(m_pPlayerFile);

	m_pGameplayFile = new QTreeWidgetItem();
	m_pGameplayFile->setIcon(0, QIcon(":/icons/gameplay-file.png"));
	m_pVideoFolder->addChild(m_pGameplayFile);

	m_pData->expandAll();
}

// +-----------------------------------------------------------
void fsdk::SessionExplorer::refreshUI()
{
	m_pRoot->setText(0, tr("Session"));
	m_pVideoFolder->setText(0, tr("Videos"));
	m_pAnnotationsFolder->setText(0, tr("Annotations"));
	m_pPlayerFile->setText(0, tr("Player"));
	m_pGameplayFile->setText(0, tr("Gameplay"));
}