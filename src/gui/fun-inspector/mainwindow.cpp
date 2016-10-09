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
#include "aboutwindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QDesktopServices>
#include <QURL>

// +-----------------------------------------------------------
fsdk::MainWindow::MainWindow(QWidget *pParent) :
    QMainWindow(pParent)
{
    setWindowIcon(QIcon(":/icons/fun-inspector.png"));

	setupUI();
}

// +-----------------------------------------------------------
fsdk::MainWindow::~MainWindow()
{
}

// +-----------------------------------------------------------
void fsdk::MainWindow::setupUI()
{
	QMenuBar *pMenuBar = new QMenuBar(this);
	setMenuBar(pMenuBar);

	//-------------------------------
	// "File" menu/toolbar
	//-------------------------------
	m_pFileMenu = menuBar()->addMenu("");
	m_pFileToolbar = addToolBar("");
	m_pFileToolbar->setIconSize(QSize(32, 32));

	// Action "Open"
	m_pOpenAction = m_pFileMenu->addAction("");
	m_pFileToolbar->addAction(m_pOpenAction);
	m_pOpenAction->setIcon(QIcon(":/icons/open.png"));
	m_pOpenAction->setShortcut(QKeySequence::Open);
	connect(m_pOpenAction, &QAction::triggered, this, &MainWindow::openSession);

	// Action "Save"
	m_pSaveAction = m_pFileMenu->addAction("");
	m_pFileToolbar->addAction(m_pSaveAction);
	m_pSaveAction->setIcon(QIcon(":/icons/save.png"));
	m_pSaveAction->setShortcut(QKeySequence::Save);
	connect(m_pSaveAction, &QAction::triggered, this, &MainWindow::saveSession);

	// Action "Save As"
	m_pSaveAsAction = m_pFileMenu->addAction("");
	m_pSaveAsAction->setIcon(QIcon(":/icons/save-as.png"));
	m_pSaveAsAction->setShortcut(QKeySequence::SaveAs);
	connect(m_pSaveAsAction, &QAction::triggered, this, &MainWindow::saveSessionAs);

	m_pFileMenu->addSeparator();

	// Action "Exit"
	m_pExitAction = m_pFileMenu->addAction("");
	m_pExitAction->setShortcut(QKeySequence::Quit);
	connect(m_pExitAction, &QAction::triggered, this, &MainWindow::quit);

	//-------------------------------
	// "Help" menu
	//-------------------------------
	m_pHelpMenu = menuBar()->addMenu("");

	// Action "Help"
	m_pHelpAction = m_pHelpMenu->addAction("");
	m_pHelpAction->setShortcut(QKeySequence::HelpContents);
	connect(m_pHelpAction, &QAction::triggered, this, &MainWindow::help);

	refreshUI();
}

// +-----------------------------------------------------------
void fsdk::MainWindow::refreshUI()
{
	//-------------------------------
	// "File" menu/toolbar
	//-------------------------------
	m_pFileMenu->setTitle(tr("&File"));
	m_pFileToolbar->setWindowTitle(tr("&File"));

	// Action "Open"
	m_pOpenAction->setText(tr("&Open session..."));
	m_pOpenAction->setStatusTip(tr("Opens a session file"));

	// Action "Save"
	m_pSaveAction->setText(tr("&Save session"));
	m_pSaveAction->setStatusTip(tr("Saves the current session"));

	// Action "Save As"
	m_pSaveAsAction->setText(tr("Save session &as..."));
	m_pSaveAsAction->setStatusTip(tr("Saves the current session to a different file"));

	// Action "Exit"
	m_pExitAction->setText(tr("&Exit"));
	m_pExitAction->setStatusTip(tr("Quits from the application"));
	
	//-------------------------------
	// "Help" menu
	//-------------------------------
	m_pHelpMenu->setTitle(tr("&Help"));

	// Action "Help"
	m_pHelpAction->setText(tr("&Help"));
	m_pHelpAction->setStatusTip(tr("Opens a browser to display the online help"));
}

// +-----------------------------------------------------------
void fsdk::MainWindow::openSession()
{

}

// +-----------------------------------------------------------
void fsdk::MainWindow::saveSession()
{

}

// +-----------------------------------------------------------
void fsdk::MainWindow::saveSessionAs()
{

}

// +-----------------------------------------------------------
void fsdk::MainWindow::quit()
{
	exit(0);
}

// +-----------------------------------------------------------
void fsdk::MainWindow::help()
{
	QDesktopServices::openUrl(QUrl("https://github.com/luigivieira/fun-sdk"));
}