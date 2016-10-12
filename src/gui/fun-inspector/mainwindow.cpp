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

#include "mainwindow.h"
#include "application.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QDesktopServices>
#include <QURL>
#include <QMdiArea>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

// +-----------------------------------------------------------
fsdk::MainWindow::MainWindow(QWidget *pParent) :
    QMainWindow(pParent)
{
	setCentralWidget(new QMdiArea(this));
	static_cast<QMdiArea*>(centralWidget())->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	static_cast<QMdiArea*>(centralWidget())->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);	

    setWindowIcon(QIcon(":/icons/fun-inspector.png"));
	setWindowTitle(QString("[*]%1").arg(qApp->applicationName()));

	m_pSessionData = new Session(this);

	setupUI();
	refreshUI();
}

// +-----------------------------------------------------------
void fsdk::MainWindow::setupUI()
{
	setDockNestingEnabled(true);

	setMenuBar(new QMenuBar(this));
	setStatusBar(new QStatusBar(this));

	//-------------------------------
	// Session explorer window
	//-------------------------------
	m_pSessionExplorer = new SessionExplorer(m_pSessionData, this);
	m_pSessionExplorer->setObjectName("sessionExplorer");
	addDockWidget(Qt::LeftDockWidgetArea, m_pSessionExplorer, Qt::Horizontal);

	//-------------------------------
	// Video windows
	//-------------------------------
	m_pPlayerWindow = new VideoWindow(this);
	m_pPlayerWindow->setWindowIcon(QIcon(":/icons/player-window.png"));
	m_pPlayerWindow->setObjectName("playerWindow");
	static_cast<QMdiArea*>(centralWidget())->addSubWindow(m_pPlayerWindow);

	m_pGameplayWindow = new VideoWindow(this);
	m_pGameplayWindow->setWindowIcon(QIcon(":/icons/gameplay-window.png"));
	m_pGameplayWindow->setObjectName("gameplayWindow");
	static_cast<QMdiArea*>(centralWidget())->addSubWindow(m_pGameplayWindow);

	//-------------------------------
	// "Session" menu/toolbar
	//-------------------------------
	m_pSessionMenu = menuBar()->addMenu("");
	m_pSessionToolbar = addToolBar("");
	m_pSessionToolbar->setIconSize(QSize(32, 32));

	// Action "New"
	m_pNewAction = m_pSessionMenu->addAction("");
	m_pSessionToolbar->addAction(m_pNewAction);
	m_pNewAction->setIcon(QIcon(":/icons/new-session.png"));
	m_pNewAction->setShortcut(QKeySequence::New);
	connect(m_pNewAction, &QAction::triggered, this, &MainWindow::newSession);

	// Action "Open"
	m_pOpenAction = m_pSessionMenu->addAction("");
	m_pSessionToolbar->addAction(m_pOpenAction);
	m_pOpenAction->setIcon(QIcon(":/icons/open-session.png"));
	m_pOpenAction->setShortcut(QKeySequence::Open);
	connect(m_pOpenAction, &QAction::triggered, this, &MainWindow::openSession);

	// Action "Save"
	m_pSaveAction = m_pSessionMenu->addAction("");
	m_pSessionToolbar->addAction(m_pSaveAction);
	m_pSaveAction->setIcon(QIcon(":/icons/save-session.png"));
	m_pSaveAction->setShortcut(QKeySequence::Save);
	connect(m_pSaveAction, &QAction::triggered, this, &MainWindow::saveSession);

	// Action "Save As"
	m_pSaveAsAction = m_pSessionMenu->addAction("");
	m_pSaveAsAction->setIcon(QIcon(":/icons/save-session-as.png"));
	m_pSaveAsAction->setShortcut(QKeySequence::SaveAs);
	connect(m_pSaveAsAction, &QAction::triggered, this, &MainWindow::saveSessionAs);

	m_pSessionMenu->addSeparator();

	// Submenu "Files"
	m_pSessionFilesMenu = m_pSessionMenu->addMenu("");

	// Submenu for the "file" actions
	m_pSessionFilesMenu->addMenu(m_pSessionExplorer->playerFileMenu());
	m_pSessionFilesMenu->addMenu(m_pSessionExplorer->gameplayFileMenu());
	m_pSessionFilesMenu->addSeparator();
	m_pSessionFilesMenu->addMenu(m_pSessionExplorer->landmarksFileMenu());

	m_pSessionMenu->addSeparator();

	// Action "Exit"
	m_pExitAction = m_pSessionMenu->addAction("");
	m_pExitAction->setShortcut(QKeySequence::Quit);
	connect(m_pExitAction, &QAction::triggered, this, &MainWindow::quit);

	//-------------------------------
	// "View" menu
	//-------------------------------
	m_pViewMenu = menuBar()->addMenu("");

	// Submenu "Windows"
	m_pViewWindowsMenu = m_pViewMenu->addMenu("");

	// Actions for the player's face window
	m_pViewWindowsMenu->addMenu(m_pPlayerWindow->actionsMenu());

	// Action for the view of the gameplay window
	m_pViewWindowsMenu->addMenu(m_pGameplayWindow->actionsMenu());

	// Separator
	m_pViewWindowsMenu->addSeparator();

	// Action for tiling the subwindows horizontally
	m_pTileHorizontalAction = m_pViewWindowsMenu->addAction("");
	m_pTileHorizontalAction->setIcon(QIcon(":/icons/windows-tile-horizontal.png"));
	connect(m_pTileHorizontalAction, &QAction::triggered, this, &MainWindow::tileHorizontally);

	// Action for tiling the subwindows vertically
	m_pTileVerticalAction = m_pViewWindowsMenu->addAction("");
	m_pTileVerticalAction->setIcon(QIcon(":/icons/windows-tile-vertical.png"));
	connect(m_pTileVerticalAction, &QAction::triggered, this, &MainWindow::tileVertically);

	// Submenu "Toolbars"
	m_pViewToolbarsMenu = m_pViewMenu->addMenu("");

	// Action for toggling the view of the File toolbar
	m_pViewToolbarsMenu->addAction(m_pSessionToolbar->toggleViewAction());

	//-------------------------------
	// "Help" menu
	//-------------------------------
	m_pHelpMenu = menuBar()->addMenu("");

	// Action "Help"
	m_pHelpAction = m_pHelpMenu->addAction("");
	m_pHelpAction->setShortcut(QKeySequence::HelpContents);
	connect(m_pHelpAction, &QAction::triggered, this, &MainWindow::help);

	// Action "About"
	m_pAboutAction = m_pHelpMenu->addAction("");
	connect(m_pAboutAction, &QAction::triggered, this, &MainWindow::about);
}

// +-----------------------------------------------------------
void fsdk::MainWindow::refreshUI()
{
	m_pSessionExplorer->refreshUI();

	//-------------------------------
	// Video windows
	//-------------------------------
	m_pPlayerWindow->setWindowTitle(tr("Player video"));
	m_pGameplayWindow->setWindowTitle(tr("Gameplay video"));

	//-------------------------------
	// Session explorer window
	//-------------------------------
	m_pSessionExplorer->setWindowTitle(tr("Session explorer"));

	//-------------------------------
	// "Session" menu/toolbar
	//-------------------------------
	m_pSessionMenu->setTitle(tr("&Session"));
	m_pSessionToolbar->setWindowTitle(tr("&Session"));

	// Action "New"
	m_pNewAction->setText(tr("&New"));
	m_pNewAction->setStatusTip(tr("Creates a new empty session"));

	// Action "Open"
	m_pOpenAction->setText(tr("&Open..."));
	m_pOpenAction->setStatusTip(tr("Opens a session file"));

	// Action "Save"
	m_pSaveAction->setText(tr("&Save"));
	m_pSaveAction->setStatusTip(tr("Saves the current session"));

	// Action "Save As"
	m_pSaveAsAction->setText(tr("Save &as..."));
	m_pSaveAsAction->setStatusTip(tr("Saves the current session to a different file"));

	// Submenu "Files"
	m_pSessionFilesMenu->setTitle(tr("&Files"));

	// Action "Exit"
	m_pExitAction->setText(tr("&Exit"));
	m_pExitAction->setStatusTip(tr("Quits from the application"));
	
	//-------------------------------
	// "View" menu
	//-------------------------------
	m_pViewMenu->setTitle(tr("&View"));

	// Submenu "Windows"
	m_pViewWindowsMenu->setTitle(tr("&Windows"));

	// Action for tiling the subwindows horizontally
	m_pTileHorizontalAction->setText(tr("Tile horizontally"));

	// Action for tiling the subwindows vertically
	m_pTileVerticalAction->setText(tr("Tile vertically"));

	// Submenu "Toolbars"
	m_pViewToolbarsMenu->setTitle(tr("&Toolbars"));

	//-------------------------------
	// "Help" menu
	//-------------------------------
	m_pHelpMenu->setTitle(tr("&Help"));

	// Action "Help"
	m_pHelpAction->setText(tr("&Help"));
	m_pHelpAction->setStatusTip(tr("Opens a browser to display the online help"));

	// Action "About"
	m_pAboutAction->setText(tr("&About"));
	m_pAboutAction->setStatusTip(tr("Shows information about this application"));
}

// +-----------------------------------------------------------
void fsdk::MainWindow::unloadApp() const
{
	// Save the application settings
	QSettings *pSettings = static_cast<Application*>(qApp)->settings();

	pSettings->beginGroup("mainWindow");
	pSettings->setValue("state", saveState());
	pSettings->setValue("geometry", saveGeometry());
	pSettings->setValue("lastPathUsed", m_sLastPathUsed);
	pSettings->endGroup();

	pSettings->beginGroup("playerWindow");
	pSettings->setValue("state", m_pPlayerWindow->saveState());
	pSettings->setValue("geometry", m_pPlayerWindow->saveGeometry());
	pSettings->endGroup();

	pSettings->beginGroup("gameplayWindow");
	pSettings->setValue("state", m_pGameplayWindow->saveState());
	pSettings->setValue("geometry", m_pGameplayWindow->saveGeometry());
	pSettings->endGroup();

	// Close all subwindows
	m_pPlayerWindow->close();
	m_pGameplayWindow->close();
}

// +-----------------------------------------------------------
void fsdk::MainWindow::loadApp()
{
	// By default, the subwindows are tiled horizontally
	tileHorizontally();

	// Read the application settings
	QSettings *pSettings = static_cast<Application*>(qApp)->settings();

	pSettings->beginGroup("mainWindow");
	restoreState(pSettings->value("state").toByteArray());
	restoreGeometry(pSettings->value("geometry").toByteArray());
	m_sLastPathUsed = pSettings->value("lastPathUsed", QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))).toString();
	pSettings->endGroup();

	pSettings->beginGroup("playerWindow");
	m_pPlayerWindow->restoreState(pSettings->value("state").toByteArray());
	m_pPlayerWindow->restoreGeometry(pSettings->value("geometry").toByteArray());
	pSettings->endGroup();

	pSettings->beginGroup("gameplayWindow");
	m_pGameplayWindow->restoreState(pSettings->value("state").toByteArray());
	m_pGameplayWindow->restoreGeometry(pSettings->value("geometry").toByteArray());
	pSettings->endGroup();

	activateWindow(); // Foce the focus back to the main window
}

// +-----------------------------------------------------------
void fsdk::MainWindow::showEvent(QShowEvent *pEvent)
{
	QMainWindow::showEvent(pEvent);
	pEvent->accept();
	QTimer::singleShot(50, this, SLOT(loadApp()));
}

// +-----------------------------------------------------------
void fsdk::MainWindow::closeEvent(QCloseEvent *pEvent)
{
	// Checks if the session has been modified
	if(m_pSessionData->isModified())
	{
		QMessageBox::StandardButton eResp = QMessageBox::question(this, tr("Attention!"), tr("The current session has not been saved yet and if you continue all unsaved data will be discarded. Do you want to close the application nevertheless?"), QMessageBox::Yes | QMessageBox::No);
		if(eResp == QMessageBox::No)
		{
			pEvent->ignore();
			return;
		}
	}

	unloadApp();
	pEvent->accept();
}

// +-----------------------------------------------------------
void fsdk::MainWindow::tileHorizontally()
{
	QMdiArea *pArea = static_cast<QMdiArea*>(centralWidget());

	if(pArea->subWindowList().isEmpty())
		return;

	QPoint oPos(0, 0);
	foreach(QMdiSubWindow *pWindow, pArea->subWindowList())
	{
		QRect oRect(0, 0, pArea->width() / pArea->subWindowList().count(), pArea->height());
		pWindow->setGeometry(oRect);
		pWindow->move(oPos);
		oPos.setX(oPos.x() + pWindow->width());
	}
}

// +-----------------------------------------------------------
void fsdk::MainWindow::tileVertically()
{
	QMdiArea *pArea = static_cast<QMdiArea*>(centralWidget());

	if(pArea->subWindowList().isEmpty())
		return;

	QPoint oPos(0, 0);
	foreach(QMdiSubWindow *pWindow, pArea->subWindowList())
	{
		QRect oRect(0, 0, pArea->width(), pArea->height() / pArea->subWindowList().count());
		pWindow->setGeometry(oRect);
		pWindow->move(oPos);
		oPos.setY(oPos.y() + pWindow->height());
	}
}

// +-----------------------------------------------------------
QString fsdk::MainWindow::lastPathUsed() const
{
	return m_sLastPathUsed;
}

// +-----------------------------------------------------------
void fsdk::MainWindow::setLastPathUsed(const QString &sPath)
{
	m_sLastPathUsed = sPath;
}

// +-----------------------------------------------------------
void fsdk::MainWindow::newSession()
{
	if(m_pSessionData->isModified())
	{
		QMessageBox::StandardButton eResp = QMessageBox::question(this, tr("Attention!"), tr("The current session has not been saved yet and if you continue all unsaved data will be discarded. Do you want to create a new empty session nevertheless?"), QMessageBox::Yes | QMessageBox::No);
		if(eResp == QMessageBox::No)
			return;
	}

	m_pSessionData->clear();
}

// +-----------------------------------------------------------
bool fsdk::MainWindow::openSession()
{
	QString sFile = QFileDialog::getOpenFileName(this, tr("Open session..."), m_sLastPathUsed, tr("Session files (*.session);; All files (*.*)"));
	if(sFile.length())
	{
		if(!m_pSessionData->load(sFile))
		{
			QApplication::beep();
			QMessageBox::critical(this, tr("Attention!"), tr("It was not possible to load the session to file %1. Please check the log file for details.").arg(m_pSessionData->sessionFileName()), QMessageBox::Ok);
			return false;
		}
		else
		{
			m_sLastPathUsed = QFileInfo(sFile).absolutePath();
			return true;
		}
	}
	return false;
}

// +-----------------------------------------------------------
bool fsdk::MainWindow::saveSession()
{
	if(m_pSessionData->sessionFileName().isEmpty())
		return saveSessionAs();
	else
	{
		if(!m_pSessionData->save(m_pSessionData->sessionFileName()))
		{
			QApplication::beep();
			QMessageBox::critical(this, tr("Attention!"), tr("It was not possible to save the session to file %1. Please check the log file for details.").arg(m_pSessionData->sessionFileName()), QMessageBox::Ok);
			return false;
		}
		else
			return true;
	}
}

// +-----------------------------------------------------------
bool fsdk::MainWindow::saveSessionAs()
{
	QString sFile = QFileDialog::getSaveFileName(this, tr("Save session..."), m_sLastPathUsed, tr("Session files (*.session);; All files (*.*)"));
	if(sFile.length())
	{
		if(!m_pSessionData->save(sFile))
		{
			QApplication::beep();
			QMessageBox::critical(this, tr("Attention!"), tr("It was not possible to save the session to file %1. Please check the log file for details.").arg(sFile), QMessageBox::Ok);
			return false;
		}
		else
		{
			m_sLastPathUsed = QFileInfo(sFile).absolutePath();
			return true;
		}
	}
	else
		return false;
}

// +-----------------------------------------------------------
void fsdk::MainWindow::quit()
{
	qWarning().noquote() << "Local: " << m_pPlayerWindow->pos() << " Global: " << m_pPlayerWindow->mapToGlobal(m_pPlayerWindow->pos());
	//close();
}

// +-----------------------------------------------------------
void fsdk::MainWindow::help()
{
	QDesktopServices::openUrl(QUrl("https://github.com/luigivieira/fun-sdk"));
}

// +-----------------------------------------------------------
void fsdk::MainWindow::about()
{

}