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

#ifndef ABSTRACTDATA_H
#define ABSTRACTDATA_H

#include "libexport.h"
#include <QMetaType>
#include <QDebug>

namespace fsdk
{
	/**
	 * Base abstract class that represents the features extracted from the videos
	 * of the player's faces.
	 */
	class SHARED_LIB_EXPORT AbstractData
	{
	public:

		/**
		 * Class constructor.
		 */
		AbstractData();

		/**
		 * Copy constructor.
		 * @param oOther Const reference to the other AbstractData object from
		 * which to copy the contents.
		 */
		AbstractData(const AbstractData& oOther);

		/**
		 * Class destructor.
		 */
		virtual ~AbstractData();
		
	};
}

// Declare the class as a Qt metatype
Q_DECLARE_METATYPE(fsdk::AbstractData);

/**
 * Overload of the QDebug::operator<< so this class can be
 * easily printed.
 * @param oDbg QDebug object to handle the printing.
 * @param oData Reference to a LandmarkData object with
 * the contents to be printed.
 * @return Object QDebug that will print the object contents.
 */
QDebug operator<<(QDebug oDbg, const fsdk::AbstractData &oData);

#endif // ABSTRACTDATA_H