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
#include <QMediaPlayer>

namespace fsdk
{
    /**
     * Handles the access to session data.
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

			/** Indicates that the video file defined is opened. */
			Opened,

			/** Indicates that the video file defined is closed. */
			Closed
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


		/**
		 * Gets the name of the landmarks file.
		 * @return QString with the name of the landmarks file.
		 */
		QString landmarksFileName() const;

		/**
		 * Sets the name of the landmarks file.
		 * @param sFileName QString with the name of the landmarks file.
		 * If it is an empty string, the current landmarks file is removed
		 * from the session.
		 */
		void setLandmarksFileName(const QString &sFileName);

		/**
		 * Clears the current session data.
		 */
		void clear();

		/**
		 * Saves the session to a file.
		 * @param sFileName QString with the name of the file to save to.
		 * @return Boolean indicating if the operation was succesful (true)
		 * or not (false).
		 */
		bool save(const QString &sFileName);

		/**
		 * Loads the session from a file.
		 * @param sFileName QString with the name of the file to load from.
		 * @return Boolean indicating if the operation was succesful (true)
		 * or not (false).
		 */
		bool load(const QString &sFileName);

		/**
		 * Indicates if the session has been modified since last save/load.
		 * @return Boolean indicating if session has been modified (true)
		 * or not (false).
		 */
		bool isModified() const;

		/**
		 * Gets the current session file name (if saved/loaded).
		 * @return QString with the name of the session file or empty if it
		 * is a new session.
		 */
		QString sessionFileName() const;

	signals:

		/**
		 * Signal indicating changes in the session contents and saved status.
		 * @param sSessionFileName QString with the name of the session file in disk (or empty
		 * if it is a new session).
		 * @param sPlayerFileName QString with the current value of the corresponding field.
		 * @param sGameplayFileName QString with the current value of the corresponding field.
		 * @param sLandmarksFileName QString with the current value of the corresponding field.
		 */
		void sessionChanged(const QString &sSessionFileName, const QString &sPlayerFileName, const QString &sGameplayFileName, const QString &sLandmarksFileName);

	protected:

		/**
		 * Sets the modified flag and emit the signal of modified changed.
		 * @param bModified Boolean indicating the current modified flag.
		 */
		void setModified(const bool bModified);

    private:

		/** Name of the file where the session data is stored. */
		QString m_sSessionFileName;

		/** Name of the video file with the player's face. */
		QString m_sPlayerFileName;

		/** Status of the video file with the player's face. */
		VideoStatus m_ePlayerStatus;

		/** Name of the video file with the gameplay session. */
		QString m_sGameplayFileName;

		/** Name of the CSV file with the facial landmarks. */
		QString m_sLandmarksFileName;

		/** Status of the video file with the gameplay session. */
		VideoStatus m_eGameplayStatus;

		/** Indicates if the session has been modified. */
		bool m_bModified;
    };
}

#endif // SESSIONEXPLORER_H