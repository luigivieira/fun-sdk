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
		 * Removes a media player from the synchronization.
		 * @param pMediaPlayer Instance of the QMediaPlayer to be synchronized.
		 */
		void remove(QMediaPlayer *pMediaPlayer);

		/**
		 * Removas all current media players from synchronization.
		 */
		void removeAll();

	signals:


	public slots:


	protected:


	private:

		/** Vector of all media players kept in sync. */
		QVector<QMediaPlayer*> m_vMedias;

	};
};

#endif // MEDIASYNCHRONIZER_H