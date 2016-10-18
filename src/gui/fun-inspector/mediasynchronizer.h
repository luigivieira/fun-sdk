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

#ifndef MEDIASYNCHRONIZER_H
#define MEDIASYNCHRONIZER_H

#include <QObject>
#include <QVector>
#include <QMediaPlayer>
#include <QAction>

namespace fsdk
{
    /**
     * Synchronizes the playback of multiple media players.
     */
    class MediaSynchronizer : public QObject
    {
        Q_OBJECT

		public:
			/**
			 * Class constructor.
			 * @param pParent Instance of the parent object.
			 * The default is NULL.
			 */
			MediaSynchronizer(QObject *pParent = 0);

			/** 
			 * Adds a media player to be synchronized.
			 * @param pMediaPlayer Instance of the QMediaPlayer to be synchronized.
			 */
			void add(QMediaPlayer *pMediaPlayer);

			/**
			 * Queries the playback state of the synchronized media.
			 * @return Value of the QMediaPlayer::State enum with the playback state.
			 */
			QMediaPlayer::State state() const;

			/**
			 * Gets the action used to toggle playing/pausing the playback.
			 * @return Instance of the QAction with that action.
			 */
			QAction *tooglePlayPauseAction() const;

			/**
			 * Gets the action used to stop the playback.
			 * @return Instance of the QAction with that action.
			 */
			QAction *stopAction() const;

		signals:

			/**
			 * Signal indicating that the playback for all media has changed.
			 * @param eState Value of the QMediaPlayer::State enum with the 
			 * playback state of all media.
			 */
			void stateChanged(QMediaPlayer::State eState);

		protected slots :

			/**
			 * Captures individual indications of playback changes in each media.
			 * @param eState Value of the QMediaPlayer::State enum with the
			 * playback state of of the sender media player.
			 */
			void onStateChanged(QMediaPlayer::State eState);

			/**
			 * Captures individual changes in the contents of a media player.
			 * @param oMedia QMediaContent with the new content of the sender
			 * media player.
			 */
			void onCurrentMediaChanged(const QMediaContent &oMedia);

			/**
			 * Captures individual changes in position of a media player.
			 * @param iPosition Long integer with the position of playback
			 * in miliseconds for the sender media player.
			 */
			void onPositionChanged(qint64 iPosition);

		public slots :

			/**
			 * Plays the videos of all media players.
			 */
			void play();

			/**
			 * Pauses the videos of all media players.
			 * If the videos are stopped when pause is requested,
			 * nothing happens.
			 */
			void pause();

			/**
			 * Toggles the playback of all media players between play/pause.
			 */
			void tooglePlayPause();

			/**
			 * Stops the videos of all media players.
			 */
			void stop();

		protected:

			/**
			 * Refreshes the text in UI elements (so translation changes
			 * can be applied).
			 */
			void refreshUI();

		private:

			/** State of the playback. */
			QMediaPlayer::State m_eState;

			/** List of all media players kept in sync. */
			QList<QMediaPlayer*> m_lMediaPlayers;

			/** List of medias pending to move to the PlayingState. */
			QList<QMediaPlayer*> m_lPendingPlay;

			/** List of medias pending to move to the PausedState. */
			QList<QMediaPlayer*> m_lPendingPause;

			/** List of medias pending to move to the StoppedState. */
			QList<QMediaPlayer*> m_lPendingStop;

			/** Action used to toggle playing/pausing the playback. */
			QAction *m_pTooglePlayPauseAction;

			/** Action used to stop the playback. */
			QAction *m_pStopAction;
	};
};

#endif // MEDIASYNCHRONIZER_H