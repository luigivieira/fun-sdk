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

#ifndef GABORDATA_H
#define GABORDATA_H

#include "libexport.h"
#include <QMap>
#include <QList>
#include <QPoint>
#include <QMetaType>
#include <QDebug>

namespace fsdk
{
	/**
	 * Represents Gabor responses extracted from facial landmarks.
	 */
	class SHARED_LIB_EXPORT GaborData
	{
	public:

		/**
		 * Class constructor.
		 */
		GaborData();

		/**
		 * Copy constructor.
		 * @param oOther Const reference to the other GaborData object from
		 * which to copy the contents.
		 */
		GaborData(const GaborData& oOther);

		/**
		 * Class destructor.
		 */
		~GaborData();

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

		/**
		 * Saves the landmarks data to the given CSV file.
		 * @param sFilename QString with the name of the file
		 * to save the data to.
		 * @return Boolean indicating if the saving was succesful
		 * (true) or not (false).
		 */
		bool saveToCSV(const QString &sFilename) const;

		/**
		 * Reads the landmarks data from the given CSV file.
		 * @param sFilename QString with the name of the file
		 * to read the data from.
		 * @return Boolean indicating if the reading was succesful
		 * (true) or not (false).
		 */
		bool readFromCSV(const QString &sFilename);

		/**
		 * Gets the maximum number of landmarks stored in this
		 * object.
		 * @return Integer with the maximum number of landmarks.
		 */
		int landmarksCount() const;

	private:

		/** Mapping between frame number and list of 2D points of landmarks. */
		QMap<int, QList<QPoint>> m_mLandmarks;
		
		/** Number of landmarks used by the tracker. */
		int m_iLandmarksCount;

	};
}

// Declare the class as a Qt metatype
Q_DECLARE_METATYPE(fsdk::GaborData);

/**
 * Overload of the QDebug::operator<< so this class can be
 * easily printed.
 * @param oDbg QDebug object to handle the printing.
 * @param oData Reference to a GaborData object with
 * the contents to be printed.
 * @return Object QDebug that will print the object contents.
 */
QDebug operator<<(QDebug oDbg, const fsdk::GaborData &oData);

#endif // GABORDATA_H