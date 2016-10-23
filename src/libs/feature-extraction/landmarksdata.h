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

#ifndef LANDMARKSDATA_H
#define LANDMARKSDATA_H

#include "libexport.h"
#include <QMap>
#include <QList>
#include <QPoint>
#include <QMetaType>
#include <QDebug>

namespace fsdk
{
	/**
	 * Represents the data of landmark points and tracker qualities extracted
	 * from videos with the facial expressions of players.
	 */
	class SHARED_LIB_EXPORT LandmarksData
	{
	public:

		/**
		 * Class constructor.
		 */
		LandmarksData();

		/**
		 * Copy constructor.
		 * @param oOther Const reference to the other LandmarkData object from
		 * which to copy the contents.
		 */
		LandmarksData(const LandmarksData& oOther);

		/**
		 * Class destructor.
		 */
		~LandmarksData();

		/**
		 * Queries if the landmarks data is empty.
		 * @return Boolean indicating if the data is empty (true) or not (false).
		 */
		bool isEmpty() const;

		/**
		 * Queries the number of frames in the data.
		 * @return Integer with the number of frames in the data.
		 */
		int count() const;

		/**
		 * Gets the tracking quality for the given frame.
		 * @param iFrame Integer with the index of the frame
		 * to get the quality for.
		 * @return Float with the quality value in range [0, 1].
		 * The return might also be 0.0 if the frame is invalid
		 * or if the data is empty.
		 */
		float quality(const int iFrame) const;

		/**
		 * Sets the tracking quality for the given frame.
		 * @param iFrame Integer with the index of the frame
		 * to get the quality for.
		 * @param fValue Float with the quality value in range [0, 1].
		 */
		void setQuality(const int iFrame, const float fValue);

		/**
		 * Queries the landmarks of a given frame. Same as operator[].
		 * @param iFrame Integer with the index of the frame to get the
		 * landmarks for.
		 * @return QList of QPoint objects with the coordinates of the
		 * facial landmarks in the given frame, or an empty QList() if
		 * the frame is invalid or if the data is empty.
		 */
		const QList<QPoint> landmarks(int iFrame) const;

		/**
		 * Adds data of the given frame.
		 * @param iFrame Integer with the number of the video frame.
		 * @param lPoints QList of QPoint objects with the coordinates of 
		 * the facial landmarks in that frame.
		 * @param fQuality Float value with the tracking quality in that frame
		 * in range [0, 1].
		 */
		void add(const int iFrame, const QList<QPoint> &lPoints, const float fQuality);

		/**
		 * Removes the data from the given frame.
		 * @param iFrame Integer with the number of the video frame.
		 */
		void remove(const int iFrame);

		/**
		 * Removes all existing frame data.
		 */
		void clear();

		bool saveToCSV(const QString &sFilename) const;

		bool readFromCSV(const QString &sFilename);

	private:

		/** Mapping between frame number and list of 2D points of landmarks. */
		QMap<int, QList<QPoint>> m_mLandmarks;

		/** Mapping between frame number and tracking quality levels. */
		QMap<int, float> m_mQualities;
		
	};
}

// Declare the class as a Qt metatype
Q_DECLARE_METATYPE(fsdk::LandmarksData);

/**
 * Overload of the QDebug::operator<< so this class can be
 * easily printed.
 * @param oDbg QDebug object to handle the printing.
 * @param oData Reference to a LandmarkData object with
 * the contents to be printed.
 * @return Object QDebug that will print the object contents.
 */
QDebug operator<<(QDebug oDbg, const fsdk::LandmarksData &oData);

#endif // LANDMARKSDATA_H