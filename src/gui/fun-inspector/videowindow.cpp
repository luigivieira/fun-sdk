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

#include "videowindow.h"
#include <QBoxLayout>
#include <QStyle>
#include <QPalette>
#include <QMainWindow>
#include <QMdiArea>
#include <QApplication>
#include <QScreen>
#include <QDebug>

// +-----------------------------------------------------------
fsdk::VideoWindow::VideoWindow(QWidget *pParent) :
	QMdiSubWindow(pParent)
{
	m_pParent = pParent;
	setWindowFlags(windowFlags() & ~Qt::WindowMinimizeButtonHint);
	
	setupUI();
	refreshUI();

	m_pMediaPlayer = new QMediaPlayer(this);
	m_pMediaPlayer->setVideoOutput(m_pFrame->graphicsVideoItem());
	connect(m_pMediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &VideoWindow::mediaStatusChanged);
	connect(m_pMediaPlayer, &QMediaPlayer::mediaStatusChanged, m_pFrame, &FrameWidget::mediaStatusChanged);
	connect(m_pMediaPlayer, &QMediaPlayer::stateChanged, this, &VideoWindow::mediaStateChanged);
	connect(m_pMediaPlayer, &QMediaPlayer::positionChanged, this, &VideoWindow::mediaPositionChanged);
}

// +-----------------------------------------------------------
void fsdk::VideoWindow::setupUI()
{
	m_pToolbar = new QToolBar(this);
	layout()->addWidget(m_pToolbar);

	m_pFrame = new FrameWidget(this);
	layout()->addWidget(m_pFrame);

	/***********************************************
	 * Toggle actions
	 ***********************************************/
	m_pActionsMenu = new QMenu(this);

	m_pToggleViewAction = m_pActionsMenu->addAction("");
	m_pToggleViewAction->setCheckable(true);
	m_pToggleViewAction->setChecked(true);
	m_pToggleViewAction->setIcon(QIcon(":/icons/window-visible.png"));
	connect(m_pToggleViewAction, &QAction::triggered, this, &VideoWindow::toggleVisible);
	

	m_pToggleDetachedAction = m_pActionsMenu->addAction("");
	m_pToggleDetachedAction->setCheckable(true);
	m_pToggleDetachedAction->setIcon(QIcon(":/icons/window-attached.png"));
	m_pToolbar->addAction(m_pToggleDetachedAction);
	connect(m_pToggleDetachedAction, &QAction::triggered, this, &VideoWindow::toggleDetached);
}

// +-----------------------------------------------------------
void fsdk::VideoWindow::refreshUI()
{
	if(m_pToggleViewAction->isChecked())
	{
		m_pToggleViewAction->setText(tr("Visible"));
		m_pToggleViewAction->setIcon(QIcon(":/icons/window-visible.png"));
	}
	else
	{
		m_pToggleViewAction->setText(tr("Hidden"));
		m_pToggleViewAction->setIcon(QIcon(":/icons/window-hidden.png"));
	}
	m_pToggleViewAction->setStatusTip(tr("Toggles the visibility of this window"));

	if(m_pToggleDetachedAction->isChecked())
	{
		m_pToggleDetachedAction->setText(tr("Detached"));
		m_pToggleDetachedAction->setIcon(QIcon(":/icons/window-detached.png"));
	}
	else
	{
		m_pToggleDetachedAction->setText(tr("Attached"));
		m_pToggleDetachedAction->setIcon(QIcon(":/icons/window-attached.png"));
	}
	m_pToggleDetachedAction->setStatusTip(tr("Toggles the detachment of this window from the main window"));
}

// +-----------------------------------------------------------
bool fsdk::VideoWindow::event(QEvent *pEvent)
{
	switch(pEvent->type())
	{
		case QEvent::Show:
			m_pToggleViewAction->setChecked(true);
			refreshUI();
			break;

		case QEvent::Close:
			m_pToggleViewAction->setChecked(false);
			refreshUI();
			break;

		case QEvent::Move:
		case QEvent::Resize:
		case QEvent::WindowStateChange:
			if(!isMaximized())
			{
				m_oGeometry = geometry();
				if(isDetached())
				{
					QMainWindow *pMainWindow = static_cast<QMainWindow*>(m_pParent);
					QMdiArea *pArea = static_cast<QMdiArea*>(pMainWindow->centralWidget());
					QPoint oPos = pArea->mapFromGlobal(m_oGeometry.topLeft());
					QSize oSize = size();
					m_oGeometry = QRect(oPos, oSize);
				}
			}
			break;

		case QEvent::WindowTitleChange:
			m_pActionsMenu->setTitle(windowTitle());
			break;
	}

	return QMdiSubWindow::event(pEvent);
}

// +-----------------------------------------------------------
void fsdk::VideoWindow::toggleVisible(bool bVisible)
{
	m_pToggleViewAction->setChecked(bVisible);

	setVisible(bVisible);
	refreshUI();
}

// +-----------------------------------------------------------
void fsdk::VideoWindow::toggleDetached(bool bDetached)
{
	m_pToggleDetachedAction->setChecked(bDetached);

	QMainWindow *pMainWindow = static_cast<QMainWindow*>(m_pParent);
	QMdiArea *pArea = static_cast<QMdiArea*>(pMainWindow->centralWidget());
	if(bDetached)
	{
		QPoint oPos = static_cast<QWidget*>(pArea)->mapToGlobal(pos());
		pArea->removeSubWindow(this);
		setParent(NULL);
		move(oPos);
	}
	else
	{
		QPoint oPos = pArea->mapFromGlobal(pos());
		setParent(m_pParent);
		pArea->addSubWindow(this);
		move(oPos);
	}

	if(m_pToggleViewAction->isChecked())
		show();
	refreshUI();
	activateWindow(); // Foce the focus back on this window
}

// +-----------------------------------------------------------
QSize fsdk::VideoWindow::sizeHint() const
{
	return QSize(800, 600);
};

// +-----------------------------------------------------------
QAction *fsdk::VideoWindow::toggleViewAction() const
{
	return m_pToggleViewAction;
}

// +-----------------------------------------------------------
QAction *fsdk::VideoWindow::toggleDetachedAction() const
{
	return m_pToggleDetachedAction;
}

// +-----------------------------------------------------------
QMenu *fsdk::VideoWindow::actionsMenu() const
{
	return m_pActionsMenu;
}

// +-----------------------------------------------------------
bool fsdk::VideoWindow::isDetached() const
{
	return m_pToggleDetachedAction->isChecked();
}

// +-----------------------------------------------------------
QByteArray fsdk::VideoWindow::saveState() const
{
	QByteArray oData;
	QDataStream oStream(&oData, QIODevice::WriteOnly);

	oStream << isDetached();
	oStream << isVisible();
	oStream << isMaximized();

	return oData;
}

// +-----------------------------------------------------------
bool fsdk::VideoWindow::restoreState(const QByteArray &oData)
{
	if(oData.isEmpty())
		return false;

	QByteArray oLocalData = oData;
	QDataStream oStream(&oLocalData, QIODevice::ReadOnly);

	bool bDetached;
	oStream >> bDetached;
	if(oStream.status() != QDataStream::Ok)
	{
		qWarning().noquote() << "Error restoring window state due to failure in reading value 'detached'";
		return false;
	}

	bool bVisible;
	oStream >> bVisible;
	if(oStream.status() != QDataStream::Ok)
	{
		qWarning().noquote() << "Error restoring window state due to failure in reading value 'visible'";
		return false;
	}
	
	bool bMaximized;
	oStream >> bMaximized;
	if(oStream.status() != QDataStream::Ok)
	{
		qWarning().noquote() << "Error restoring window state due to failure in reading value 'maximized'";
		return false;
	}

	if(bDetached)
		toggleDetached(true);
	if(!bVisible)
		toggleVisible(false);
	if(bMaximized)
		setWindowState(Qt::WindowMaximized);

	return true;
}

// +-----------------------------------------------------------
QByteArray fsdk::VideoWindow::saveGeometry() const
{
	QByteArray oData;
	QDataStream oStream(&oData, QIODevice::WriteOnly);
	oStream << m_oGeometry;
	return oData;
}

// +-----------------------------------------------------------
bool fsdk::VideoWindow::restoreGeometry(const QByteArray &oData)
{
	if(oData.isEmpty())
		return false;

	QByteArray oLocalData = oData;
	QDataStream oStream(&oLocalData, QIODevice::ReadOnly);

	oStream >> m_oGeometry;
	if(oStream.status() != QDataStream::Ok)
	{
		qWarning().noquote() << "Error restoring window geometry due to failure in reading value 'geometry'";
		return false;
	}

	setGeometry(m_oGeometry);
	return true;
}

// +-----------------------------------------------------------
void fsdk::VideoWindow::mediaStatusChanged(QMediaPlayer::MediaStatus eStatus)
{
	qDebug().noquote() << "Media status changed to: " << eStatus;
}

// +-----------------------------------------------------------
void fsdk::VideoWindow::mediaStateChanged(QMediaPlayer::State eState)
{
	qDebug().noquote() << "Media state changed to: " << eState;

}

// +-----------------------------------------------------------
void fsdk::VideoWindow::mediaPositionChanged(qint64 iPosition)
{

}

// +-----------------------------------------------------------
void fsdk::VideoWindow::setVideoFile(const QString &sFileName)
{
	QUrl oUrl = QUrl::fromLocalFile(sFileName);
	QMediaContent oMedia = QMediaContent(oUrl);
	m_pMediaPlayer->setMedia(oMedia);
}

// +-----------------------------------------------------------
void fsdk::VideoWindow::play()
{

}

// +-----------------------------------------------------------
void fsdk::VideoWindow::stop()
{

}
