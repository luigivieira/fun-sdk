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

#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QFile>
#include <opencv2\opencv.hpp>

using namespace cv;

namespace fsdk
{
    /**
     * Session data model class.
     */
    class Session : public QObject
    {
        Q_OBJECT

    public:
		/** Indicates the status of a video file. */
		enum VideoStatus {
			/** Indicates that there is no video defined. */
			NotDefined,

			/** Indicates that the video file defined could not be read. */
			ReadError,

			/** Indicates that the video file defined was opened ok. */
			Ok
		};
		Q_ENUM(VideoStatus);

        /**
         * Class constructor.
         * @param pParent QObject with the parent object.
         */
		Session(QObject *pParent = 0);

		/**
		 * Gets the name of the video file with the player's face.
		 * @return QString with the name of the player's video file.
		 */
		QString playerFileName() const;

		/**
		 * Sets the name of the video file with the player's face.
		 * @param sFileName QString with the name of the player's video file.
		 * If it is an empty string, the current video file is removed from
		 * the session.
		 */
		void setPlayerFileName(const QString &sFileName);

		/**
		 * Gets the name of the video file with the gameplay.
		 * @return QString with the name of the gameplay video file.
		 */
		QString gameplayFileName() const;

		/**
		 * Sets the name of the video file with the gameplay.
		 * @param sFileName QString with the name of the gameplay video file.
		 * If it is an empty string, the current video file is removed from
		 * the session.
		 */
		void setGameplayFileName(const QString &sFileName);



		void save(const QString &sFileName);

		void load(const QString &sFileName);

    private:

		/** Name of the video file with the player's face. */
		QString m_sPlayerFileName;

		/** Opencv's capture of frames of the player video. */
		VideoCapture m_oPlayerVideo;

		/** Status of the video file with the player's face. */
		VideoStatus m_ePlayerStatus;

		/** Name of the video file with the gameplay session. */
		QString m_sGameplayFileName;

		/** Name of the CSV file with the facial landmarks. */
		QString m_sLandmarksFileName;

		/** Status of the video file with the gameplay session. */
		VideoStatus m_eGameplayStatus;


		/** Opencv's capture of frames of the gameplay video. */
		VideoCapture m_oGameplayVideo;
    };
}

#endif // SESSIONEXPLORER_H