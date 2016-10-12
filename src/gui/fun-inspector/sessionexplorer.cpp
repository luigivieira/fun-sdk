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
#include "mainwindow.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>

// +-----------------------------------------------------------
fsdk::SessionExplorer::SessionExplorer(Session *pData, MainWindow *pParent) :
	QDockWidget("", pParent)
{
	m_pMainWindow = pParent;

	m_pData = pData;

	m_pView = new TreeWidget(this);
	setWidget(m_pView);

	connect(m_pData, &Session::sessionChanged, this, &SessionExplorer::sessionChanged);

	m_pView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pView, &QTreeWidget::customContextMenuRequested, this, &SessionExplorer::showContextMenu);
	
	m_pView->setColumnCount(2);
	m_pView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_pView->header()->close();
	
	m_pRoot = new QTreeWidgetItem();
	m_pRoot->setIcon(0, QIcon(":/icons/session-folder.png"));
	m_pView->addTopLevelItem(m_pRoot);

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

	m_pView->expandAll();

	m_pPlayerFileMenu = new QMenu(this);
	m_pPlayerFileAddAction = m_pPlayerFileMenu->addAction("");
	m_pPlayerFileAddAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
	connect(m_pPlayerFileAddAction, &QAction::triggered, this, &SessionExplorer::addPlayerFile);
	m_pPlayerFileRemoveAction = m_pPlayerFileMenu->addAction("");
	m_pPlayerFileRemoveAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_1));
	connect(m_pPlayerFileRemoveAction, &QAction::triggered, this, &SessionExplorer::removePlayerFile);

	m_pGameplayFileMenu = new QMenu(this);
	m_pGameplayFileAddAction = m_pGameplayFileMenu->addAction("");
	m_pGameplayFileAddAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
	connect(m_pGameplayFileAddAction, &QAction::triggered, this, &SessionExplorer::addGameplayFile);
	m_pGameplayFileRemoveAction = m_pGameplayFileMenu->addAction("");
	m_pGameplayFileRemoveAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_2));
	connect(m_pGameplayFileRemoveAction, &QAction::triggered, this, &SessionExplorer::removeGameplayFile);

	m_pLandmarksFileMenu = new QMenu(this);
	m_pLandmarksFileAddAction = m_pLandmarksFileMenu->addAction("");
	m_pLandmarksFileAddAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
	connect(m_pLandmarksFileAddAction, &QAction::triggered, this, &SessionExplorer::addLandmarksFile);
	m_pLandmarksFileRemoveAction = m_pLandmarksFileMenu->addAction("");
	m_pLandmarksFileRemoveAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_3));
	connect(m_pLandmarksFileRemoveAction, &QAction::triggered, this, &SessionExplorer::removeLandmarksFile);
}

// +-----------------------------------------------------------
void fsdk::SessionExplorer::refreshUI()
{
	m_pRoot->setText(0, tr("%1Session").arg(m_pData->isModified() ? "*" : ""));
	m_pVideosFolder->setText(0, tr("Videos"));
	m_pAnnotationsFolder->setText(0, tr("Annotations"));
	m_pPlayerFile->setText(0, tr("Player"));
	m_pGameplayFile->setText(0, tr("Gameplay"));
	m_pLandmarksFile->setText(0, tr("Facial landmarks"));

	m_pPlayerFileMenu->setTitle(tr("Player video"));
	m_pPlayerFileAddAction->setText(tr("&Select..."));
	m_pPlayerFileAddAction->setIcon(QIcon(":/icons/file-add.png"));
	m_pPlayerFileAddAction->setStatusTip(tr("Select a video file of the player's face to add to the session"));
	m_pPlayerFileRemoveAction->setText(tr("&Remove"));
	m_pPlayerFileRemoveAction->setIcon(QIcon(":/icons/file-remove.png"));
	m_pPlayerFileRemoveAction->setStatusTip(tr("Remove the current video of the player's face from the session"));

	m_pGameplayFileMenu->setTitle(tr("Gameplay video"));
	m_pGameplayFileAddAction->setText(tr("&Select..."));
	m_pGameplayFileAddAction->setIcon(QIcon(":/icons/file-add.png"));
	m_pGameplayFileAddAction->setStatusTip(tr("Select a video file of the gameplay to add to the session"));
	m_pGameplayFileRemoveAction->setText(tr("&Remove"));
	m_pGameplayFileRemoveAction->setIcon(QIcon(":/icons/file-remove.png"));
	m_pGameplayFileRemoveAction->setStatusTip(tr("Remove the current video of the gameplay from the session"));

	m_pLandmarksFileMenu->setTitle(tr("Facial landmarks"));
	m_pLandmarksFileAddAction->setText(tr("&Select..."));
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

	m_pRoot->setText(0, tr("%1Session").arg(m_pData->isModified() ? "*" : ""));
	m_pMainWindow->setWindowModified(m_pData->isModified());
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

// +-----------------------------------------------------------
void fsdk::SessionExplorer::showContextMenu(const QPoint &oClickPos)
{
	QTreeWidgetItem *pItem = m_pView->itemAt(oClickPos);
	if(pItem == m_pPlayerFile)
	{
		QMenu oMenu(this);
		oMenu.addAction(m_pPlayerFileAddAction);
		oMenu.addAction(m_pPlayerFileRemoveAction);
		oMenu.exec(m_pView->mapToGlobal(oClickPos));
	}
	else if(pItem == m_pGameplayFile)
	{
		QMenu oMenu(this);
		oMenu.addAction(m_pGameplayFileAddAction);
		oMenu.addAction(m_pGameplayFileRemoveAction);
		oMenu.exec(m_pView->mapToGlobal(oClickPos));
	}
	else if(pItem == m_pLandmarksFile)
	{
		QMenu oMenu(this);
		oMenu.addAction(m_pLandmarksFileAddAction);
		oMenu.addAction(m_pLandmarksFileRemoveAction);
		oMenu.exec(m_pView->mapToGlobal(oClickPos));
	}
}

// +-----------------------------------------------------------
bool fsdk::SessionExplorer::addPlayerFile()
{	
	QString sFile = QFileDialog::getOpenFileName(this, tr("Select player file..."), m_pMainWindow->lastPathUsed(), tr("Video files (*.mp4);; All files (*.*)"));
	if(sFile.length())
	{
		m_pData->setPlayerFileName(sFile);
		m_pMainWindow->setLastPathUsed(QFileInfo(sFile).absolutePath());
		return true;
	}
	else
		return false;
}

// +-----------------------------------------------------------
bool fsdk::SessionExplorer::removePlayerFile()
{
	QMessageBox::StandardButton eResp = QMessageBox::question(this, tr("Attention!"), tr("Do you confirm removing the player video file from the session?"), QMessageBox::Yes | QMessageBox::No);
	if(eResp == QMessageBox::Yes)
	{
		m_pData->setPlayerFileName("");
		return true;
	}
	else
		return false;
}

// +-----------------------------------------------------------
bool fsdk::SessionExplorer::addGameplayFile()
{
	QString sFile = QFileDialog::getOpenFileName(this, tr("Select gameplay file..."), m_pMainWindow->lastPathUsed(), tr("Video files (*.mp4);; All files (*.*)"));
	if(sFile.length())
	{
		m_pData->setGameplayFileName(sFile);
		m_pMainWindow->setLastPathUsed(QFileInfo(sFile).absolutePath());
		return true;
	}
	else
		return false;
}

// +-----------------------------------------------------------
bool fsdk::SessionExplorer::removeGameplayFile()
{
	QMessageBox::StandardButton eResp = QMessageBox::question(this, tr("Attention!"), tr("Do you confirm removing the gameplay video file from the session?"), QMessageBox::Yes | QMessageBox::No);
	if(eResp == QMessageBox::Yes)
	{
		m_pData->setGameplayFileName("");
		return true;
	}
	else
		return false;
}

// +-----------------------------------------------------------
bool fsdk::SessionExplorer::addLandmarksFile()
{
	QString sFile = QFileDialog::getOpenFileName(this, tr("Select landmarks file..."), m_pMainWindow->lastPathUsed(), tr("Comma-Separated-Value files (*.csv);; All files (*.*)"));
	if(sFile.length())
	{
		m_pData->setLandmarksFileName(sFile);
		m_pMainWindow->setLastPathUsed(QFileInfo(sFile).absolutePath());
		return true;
	}
	else
		return false;
}

// +-----------------------------------------------------------
bool fsdk::SessionExplorer::removeLandmarksFile()
{
	QMessageBox::StandardButton eResp = QMessageBox::question(this, tr("Attention!"), tr("Do you confirm removing the landmarks CSV file from the session?"), QMessageBox::Yes | QMessageBox::No);
	if(eResp == QMessageBox::Yes)
	{
		m_pData->setLandmarksFileName("");
		return true;
	}
	else
		return false;
}