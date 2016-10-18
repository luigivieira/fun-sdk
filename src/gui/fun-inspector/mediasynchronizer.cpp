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

#include "mediasynchronizer.h"

// +-----------------------------------------------------------
fsdk::MediaSynchronizer::MediaSynchronizer(QObject *pParent) : QObject(pParent)
{
	m_eState = QMediaPlayer::StoppedState;

	m_pTooglePlayPauseAction = new QAction(this);
	m_pTooglePlayPauseAction->setShortcut(QKeySequence(Qt::Key_P));
	m_pTooglePlayPauseAction->setIcon(QIcon(":/icons/play-videos.png"));
	connect(m_pTooglePlayPauseAction, &QAction::triggered, this, &MediaSynchronizer::tooglePlayPause);

	m_pStopAction = new QAction(this);
	m_pStopAction->setIcon(QIcon(":/icons/stop-videos.png"));
	m_pStopAction->setShortcut(QKeySequence(Qt::Key_S));
	connect(m_pStopAction, &QAction::triggered, this, &MediaSynchronizer::stop);

	refreshUI();
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::refreshUI()
{
	switch(m_eState)
	{
		case QMediaPlayer::PausedState:
		case QMediaPlayer::StoppedState:
			m_pTooglePlayPauseAction->setIcon(QIcon(":/icons/play-videos.png"));
			m_pTooglePlayPauseAction->setText(tr("Play"));
			m_pTooglePlayPauseAction->setStatusTip(tr("Plays the session videos"));
			break;

		default:
			m_pTooglePlayPauseAction->setIcon(QIcon(":/icons/pause-videos.png"));
			m_pTooglePlayPauseAction->setText(tr("Pause"));
			m_pTooglePlayPauseAction->setStatusTip(tr("Pauses the session videos"));
			break;
	}
	
	m_pStopAction->setText(tr("Stop"));
	m_pStopAction->setStatusTip(tr("Stops the session videos"));
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::add(QMediaPlayer *pMediaPlayer)
{
	connect(pMediaPlayer, &QMediaPlayer::stateChanged, this, &MediaSynchronizer::onStateChanged);
	connect(pMediaPlayer, &QMediaPlayer::currentMediaChanged, this, &MediaSynchronizer::onCurrentMediaChanged);
	connect(pMediaPlayer, &QMediaPlayer::positionChanged, this, &MediaSynchronizer::onPositionChanged);
	m_lMediaPlayers.push_back(pMediaPlayer);
}

// +-----------------------------------------------------------
QMediaPlayer::State fsdk::MediaSynchronizer::state() const
{
	return m_eState;
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::play()
{
	if(m_eState == QMediaPlayer::PlayingState || m_lPendingPlay.count() != 0)
		return;

	foreach(QMediaPlayer *pMediaPlayer, m_lMediaPlayers)
		if(!pMediaPlayer->media().isNull())
			m_lPendingPlay.push_back(pMediaPlayer);

	foreach(QMediaPlayer *pMediaPlayer, m_lMediaPlayers)
		if(!pMediaPlayer->media().isNull())
			pMediaPlayer->play();
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::pause()
{
	if(m_eState == QMediaPlayer::PausedState || m_lPendingPause.count() != 0)
		return;

	if(m_eState == QMediaPlayer::StoppedState)
		return;

	foreach(QMediaPlayer *pMediaPlayer, m_lMediaPlayers)
		if(!pMediaPlayer->media().isNull())
			m_lPendingPause.push_back(pMediaPlayer);

	foreach(QMediaPlayer *pMediaPlayer, m_lMediaPlayers)
		if(!pMediaPlayer->media().isNull())
			pMediaPlayer->pause();
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::tooglePlayPause()
{
	switch(m_eState)
	{
		case QMediaPlayer::StoppedState:
		case QMediaPlayer::PausedState:
			play();
			break;

		case QMediaPlayer::PlayingState:
			pause();
			break;
	}
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::stop()
{
	if(m_eState == QMediaPlayer::StoppedState || m_lPendingStop.count() != 0)
		return;

	foreach(QMediaPlayer *pMediaPlayer, m_lMediaPlayers)
		if(!pMediaPlayer->media().isNull())
			m_lPendingStop.push_back(pMediaPlayer);

	foreach(QMediaPlayer *pMediaPlayer, m_lMediaPlayers)
		if(!pMediaPlayer->media().isNull())
			pMediaPlayer->stop();
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::onStateChanged(QMediaPlayer::State eState)
{
	QMediaPlayer *pMediaPlayer = static_cast<QMediaPlayer*>(sender());

	switch(eState)
	{
	case QMediaPlayer::PlayingState:
		m_lPendingPlay.removeOne(pMediaPlayer);
		if(m_lPendingPlay.count() == 0)
		{
			m_eState = QMediaPlayer::PlayingState;
			qDebug().noquote() << "Synchronizer state changed from: " << QMediaPlayer::PlayingState << " to: " << m_eState;
			emit stateChanged(m_eState);
			refreshUI();
		}
		break;

	case QMediaPlayer::PausedState:
		m_lPendingPause.removeOne(pMediaPlayer);
		if(m_lPendingPause.count() == 0)
		{
			m_eState = QMediaPlayer::PausedState;
			qDebug().noquote() << "Synchronizer state changed from: " << QMediaPlayer::PausedState << " to: " << m_eState;
			emit stateChanged(m_eState);
			refreshUI();
		}
		break;

	case QMediaPlayer::StoppedState:
		m_lPendingStop.removeOne(pMediaPlayer);
		if(m_lPendingStop.count() == 0)
		{
			m_eState = QMediaPlayer::StoppedState;
			qDebug().noquote() << "Synchronizer state changed from: " << QMediaPlayer::StoppedState << " to: " << m_eState;
			emit stateChanged(m_eState);
			refreshUI();
		}
		break;
	}
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::onCurrentMediaChanged(const QMediaContent &oMedia)
{
	// Simply stop all media if one of them has its contents changed
	// (it is simply easier for the user to start the playback again
	// then try to possibly resync that particular media with all existing)
	if(m_eState != QMediaPlayer::StoppedState)
		stop();
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::onPositionChanged(qint64 iPosition)
{

}

// +-----------------------------------------------------------
QAction *fsdk::MediaSynchronizer::tooglePlayPauseAction() const
{
	return m_pTooglePlayPauseAction;
}

// +-----------------------------------------------------------
QAction *fsdk::MediaSynchronizer::stopAction() const
{
	return m_pStopAction;
}