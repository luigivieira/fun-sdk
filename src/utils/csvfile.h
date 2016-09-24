/*
 * Copyright (C) 2016 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
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

#ifndef CSVFILE_H
#define CSVFILE_H

#include "libexport.h"
#include <QFile>
#include <QList>
#include <QStringList>

namespace fsdk
{
	/**
	 * Defines the basic face tracking interface.
	 * Specific implementations must inherit this class
	 * and implement its pure virtual methods.
	 */
	class LIBEXPORT CSVFile: QFile
	{
	public:
		/**
		 * Default constructor.
		 * @param iColumns Integer with the number of columns in
		 * the CSV loaded. The default is 0 (meaning that values
		 * can only be added from a file loaded from disk).
		 * @param cSeparator QChar with the separator used for
		 * loading/saving the CSV values to/from a file in the
		 * disk. The default is ';'.
		 */
		CSVFile(const int iColumns = 0, const QChar cSeparator = ';');

		/**
		 * Constructor that accepts the path and name of the CSV file to use.
		 * @param sName QString with the path and name of the CSV file.
		 * @param iColumns Integer with the number of columns in
		 * the CSV loaded. The default is 0 (meaning that values
		 * can only be added from a file loaded from disk).
		 * @param cSeparator QChar with the separator used for
		 * loading/saving the CSV values to/from a file in the
		 * disk. The default is ';'.		 */
		CSVFile(const QString &sName, const int iColumns = 0, const QChar cSeparator = ';');

		/**
		 * Loads the CSV file according to the file name currently
		 * defined in name().
		 * @param bHasHeader Boolean indicating if the CSV file has
		 * a header (true, the default value) or not (false).
		 * @return Boolean indicating if the loading was successful (true)
		 * or not (false). If the return is false, the reason is described
		 * in error(). If the return is false and the return of error()
		 * is NoError, the reason is not due to the access to the file,
		 * but to internal structural errors (i.e. different number of
		 * values between header or lines).
		 */
		bool load(const bool bHasHeader = true);

		/**
		* Loads the CSV file from the given file.
		* @param sName QString with the path and name of the file to load.
		* @param bHasHeader Boolean indicating if the CSV file has
		* a header (true, the default value) or not (false).
		* @return Boolean indicating if the loading was successful (true)
		* or not (false). If the return is false, the reason is described
		* in error(). If the return is false and the return of error()
		* is NoError, the reason is not due to the access to the file,
		* but to internal structural errors (i.e. different number of
		* values between header or lines).
		*/
		bool loadFromFile(const QString &sName, const bool bHasHeader = true);

		/**
		 * Indicates if the CSV loaded has a header.
		 * @return Boolean indicating if the CSV loaded
		 * has a header (true) or not (false).
		 */
		bool hasHeader() const;

		/**
		 * Gets the number of columns in the CSV loaded.
		 * @return Integer with the number of columns.
		 */
		int columns() const;

		/**
		 * Gets the number of rows in the CSV loaded (not considering
		 * the header, if existing).
		 * @return Integer with the number of rows.
		 */
		int rows() const;

		/**
		 * Gets the header values currently loaded (if any).
		 * @return QStringList with the values in the header
		 * of an empty QStringList if the CSV loaded has no
		 * header.
		 */
		QStringList header() const;

		/**
		 * Sets the values in the header for the currently loaded CSV.
		 * @param lHeader QStringList with the new header. It must
		 * contain the exactly same number of values as returned
		 * by columns() or no values at all (in case which the header
		 * is completely removed from the loaded CSV).
		 * @return Boolean indicating if the header has been replaced
		 * (true) or not (false). False is only returned when a non-
		 * empty list of header values is provided with a different
		 * number of values than those in columns().
		 */
		bool setHeader(const QStringList &lHeader);
	
		/**
		 * Gets all values of the currently loaded CSV.
		 * @return A QList of rows with QStringLists for the columns
		 * in the CSV data. Or a QList() if the CSV has no data.
		 */
		QList<QStringList> values() const;

		/**
		 * Gets the values of the currently loaded CSV for a given row.
		 * @param iRow Integer with the row to query. The row is counted
		 * from 0 to rows()-1.
		 * @return A QStringList with all columns in the given row, or 
		 * a QStringList() if the CSV has no data or the given row is out
		 * of range.
		 */
		QStringList rowValues(const int iRow) const;

		/**
		 * Sets all the values for the CSV currently loaded.
		 * @param lValues A QList of rows with QStringLists 
		 * with the columns of data. It can be a QList() to 
		 * completely clear the CSV data.
		 * @return A boolean indicating if the update was succesful
		 * (true) or not (false).
		 */
		bool setValues(const QList<QStringList> &lValues);

		/**
		* Sets the values for the CSV currently loaded in the
		* given row.
		* @param iRow Integer with the row to update. The row is
		* counted from 0 to rows()-1.
		* @param lValues A QStringList with the values of the
		* rows. It must have the same number of values as in
		* columns().
		* @return A boolean indicating if the update was succesful
		* (true) or not (false).
		*/
		bool setRowValues(const int iRow, const QStringList &lValues);

	private:
		
		/** Names in the CSV header. */
		QStringList m_lHeader;

		/** List of values in the CSV. */
		QList<QStringList> m_lValues;

		/** Character used for separating values in the CSV file. */
		QChar m_cSeparator;

		/** Number of columns in the CSV loaded. */
		int m_iColumns;
	};
}

#endif // CSVFILE_H