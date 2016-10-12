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
	m_pData = new TreeWidget(this);
	setWidget(m_pData);
	
	m_pData->setColumnCount(2);
	m_pData->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_pData->header()->close();
	
	m_pRoot = new QTreeWidgetItem();
	m_pRoot->setIcon(0, QIcon(":/icons/session-folder.png"));
	m_pData->addTopLevelItem(m_pRoot);

	m_pVideosFolder = new QTreeWidgetItem();
	m_pVideosFolder->setIcon(0, QIcon(":/icons/videos-folder.png"));
	m_pRoot->addChild(m_pVideosFolder);

	m_pAnnotationsFolder = new QTreeWidgetItem();
	m_pAnnotationsFolder->setIcon(0, QIcon(":/icons/annotations-folder.png"));
	m_pRoot->addChild(m_pAnnotationsFolder);

	m_pPlayerFile = new QTreeWidgetItem();
	m_pPlayerFile->setIcon(0, QIcon(":/icons/player-file.png"));
	m_pVideosFolder->addChild(m_pPlayerFile);

	m_pGameplayFile = new QTreeWidgetItem();
	m_pGameplayFile->setIcon(0, QIcon(":/icons/gameplay-file.png"));
	m_pVideosFolder->addChild(m_pGameplayFile);

	m_pLandmarksFile = new QTreeWidgetItem();
	m_pLandmarksFile->setIcon(0, QIcon(":/icons/landmarks-file.png"));
	m_pAnnotationsFolder->addChild(m_pLandmarksFile);

	m_pData->expandAll();

	m_pPlayerFileMenu = new QMenu(this);
	m_pPlayerFileAddAction = m_pPlayerFileMenu->addAction("");
	m_pPlayerFileAddAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
	m_pPlayerFileRemoveAction = m_pPlayerFileMenu->addAction("");
	m_pPlayerFileRemoveAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_1));

	m_pGameplayFileMenu = new QMenu(this);
	m_pGameplayFileAddAction = m_pGameplayFileMenu->addAction("");
	m_pGameplayFileAddAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
	m_pGameplayFileRemoveAction = m_pGameplayFileMenu->addAction("");
	m_pGameplayFileRemoveAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_2));

	m_pLandmarksFileMenu = new QMenu(this);
	m_pLandmarksFileAddAction = m_pLandmarksFileMenu->addAction("");
	m_pLandmarksFileAddAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
	m_pLandmarksFileRemoveAction = m_pLandmarksFileMenu->addAction("");
	m_pLandmarksFileRemoveAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_3));
}

// +-----------------------------------------------------------
void fsdk::SessionExplorer::refreshUI()
{
	m_pRoot->setText(0, tr("Session"));
	m_pVideosFolder->setText(0, tr("Videos"));
	m_pAnnotationsFolder->setText(0, tr("Annotations"));
	m_pPlayerFile->setText(0, tr("Player"));
	m_pGameplayFile->setText(0, tr("Gameplay"));
	m_pLandmarksFile->setText(0, tr("Facial landmarks"));

	m_pPlayerFileMenu->setTitle(tr("Player video"));
	m_pPlayerFileAddAction->setText(tr("&Add..."));
	m_pPlayerFileAddAction->setIcon(QIcon(":/icons/file-add.png"));
	m_pPlayerFileAddAction->setStatusTip(tr("Select a video file of the player's face to add to the session"));
	m_pPlayerFileRemoveAction->setText(tr("&Remove"));
	m_pPlayerFileRemoveAction->setIcon(QIcon(":/icons/file-remove.png"));
	m_pPlayerFileRemoveAction->setStatusTip(tr("Remove the current video of the player's face from the session"));

	m_pGameplayFileMenu->setTitle(tr("Gameplay video"));
	m_pGameplayFileAddAction->setText(tr("&Add..."));
	m_pGameplayFileAddAction->setIcon(QIcon(":/icons/file-add.png"));
	m_pGameplayFileAddAction->setStatusTip(tr("Select a video file of the gameplay to add to the session"));
	m_pGameplayFileRemoveAction->setText(tr("&Remove"));
	m_pGameplayFileRemoveAction->setIcon(QIcon(":/icons/file-remove.png"));
	m_pGameplayFileRemoveAction->setStatusTip(tr("Remove the current video of the gameplay from the session"));

	m_pLandmarksFileMenu->setTitle(tr("Facial landmarks"));
	m_pLandmarksFileAddAction->setText(tr("&Add..."));
	m_pLandmarksFileAddAction->setIcon(QIcon(":/icons/file-add.png"));
	m_pLandmarksFileAddAction->setStatusTip(tr("Select a landmarks file to add to the session"));
	m_pLandmarksFileRemoveAction->setText(tr("&Remove"));
	m_pLandmarksFileRemoveAction->setIcon(QIcon(":/icons/file-remove.png"));
	m_pLandmarksFileRemoveAction->setStatusTip(tr("Remove the current landmarks file from the session"));
}

// +-----------------------------------------------------------
void fsdk::SessionExplorer::sessionChanged(const QString &sSessionFileName, const QString &sPlayerFileName, const QString &sGameplayFileName, const QString &sLandmarksFileName)
{
	m_pRoot->setText(1, sSessionFileName);
	m_pPlayerFile->setText(1, sPlayerFileName);
	m_pGameplayFile->setText(1, sGameplayFileName);
	m_pLandmarksFile->setText(1, sLandmarksFileName);
}

// +-----------------------------------------------------------
QMenu *fsdk::SessionExplorer::playerFileMenu() const
{
	return m_pPlayerFileMenu;
}

// +-----------------------------------------------------------
QMenu *fsdk::SessionExplorer::gameplayFileMenu() const
{
	return m_pGameplayFileMenu;
}

// +-----------------------------------------------------------
QMenu *fsdk::SessionExplorer::landmarksFileMenu() const
{
	return m_pLandmarksFileMenu;
}