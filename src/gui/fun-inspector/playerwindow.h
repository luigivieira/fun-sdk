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

#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include "videowindow.h"
#include "landmarkwidget.h"
#include <QList>

namespace fsdk
{
    /**
     * Custom VideoWindow for showing the player video with the facial landmarks.
     */
    class PlayerWindow : public VideoWindow
    {
        Q_OBJECT

    public:
        /**
         * Class constructor.
		 * @param iLandmarks Integer with the number of landmarks to display.
		 * @param pSession Pointer to the Session handling the landmarks data.
         * @param pParent QWidget with the window parent.
         */
		PlayerWindow(int iLandmarks, QWidget *pParent);

	public slots:

		/**
		 * Captures the signal indicating updates in the landmarks file.
		 * @param sFileName QString with the name of the landmarks file
		 * in the session (empty if no file has been assigned to the session).
		 */
		void landmarksFileChanged(const QString sFileName);

	protected:

		/**
		 * Captures indications of changes in the media position.
		 * @param iPosition Long integer with the position expressed
		 * in milliseconds.
		 */
		void mediaPositionChanged(qint64 iPosition);

		bool landmarksVisible() const;

		void setLandmarksVisible(bool bVisible);

    private:

		/** List of landmarks objects in the video widget. */
		QList<LandmarkWidget*> m_lLandmarks;

		/** Mapping of the landmarks data for each frame (read from the CSV). */
		QMap<qint64, QList<QPoint>> m_mData;
    };
}

#endif // PLAYERWINDOW_H