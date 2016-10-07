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

#ifndef CSVFILE_H
#define CSVFILE_H

#include "global.h"
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
	class SHARED_LIB_EXPORT CSVFile: public QFile
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
		 * Loads the CSV from the given file.
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
		 * Saves the CSV in the file currently defined in name().
		 * @return Boolean indicating if the saving was successful (true)
		 * or not (false). If the return is false, the reason is described
		 * in error().
		 */
		bool save();

		/**
		 * Saves the CSV in the given file.
		 * @param sName QString with the path and name of the file to save.
		 * @return Boolean indicating if the saving was successful (true)
		 * or not (false). If the return is false, the reason is described
		 * in error().
		 */
		bool saveToFile(const QString &sName);

		/**
		 * Gets the separator used by the CSV.
		 * @return QChar with the separator used.
		 */
		QChar separator() const;

		/**
		 * Sets the separator used by the CSV.
		 * @param cSeparator QChar with the separator to use.
		 */
		void setSeparator(const QChar cSeparator);

		/**
		 * Indicates if the CSV has a header.
		 * @return Boolean indicating if the CSV has a
		 * header (true) or not (false).
		 */
		bool hasHeader() const;

		/**
		 * Gets the number of columns in the CSV.
		 * @return Integer with the number of columns.
		 */
		int columnsCount() const;

		/**
		 * Gets the number of rows in the CSV (not considering
		 * the header).
		 * @return Integer with the number of rows.
		 */
		int rowsCount() const;

		/**
		 * Gets the header values in the CSV (if any).
		 * @return QStringList with the values in the header
		 * of an empty QStringList if the CSV has no header.
		 */
		QStringList header() const;

		/**
		 * Sets the values in the header for the CSV.
		 * @param lHeader QStringList with the new header. It must
		 * contain the exactly same number of values as returned
		 * by columnsCount() or no values at all (in case which an
		 * existing header is completely removed from the CSV).
		 * @return Boolean indicating if the header has been replaced
		 * (true) or not (false). False is only returned when a non-
		 * empty list of header values is provided with a different
		 * number of values than those in columnsCount().
		 */
		bool setHeader(const QStringList &lHeader);
	
		/**
		 * Gets all rows of the CSV.
		 * @return A QList of rows with QStringLists for the columns
		 * in the CSV data, or a QList() if the CSV has no data.
		 */
		QList<QStringList> rows() const;

		/**
		 * Gets the values of the given row in CSV.
		 * @param iRow Integer with the row to query. The row is counted
		 * from 0 to rowsCount()-1.
		 * @return A QStringList with all values in the given row, or 
		 * a QStringList() if the CSV has no data or the given row is out
		 * of range.
		 */
		QStringList row(const int iRow) const;

		/**
		 * Sets all rows for the CSV.
		 * @param lRows A QList of QStringLists with the columns of
		 * data. It can be a QList() to completely clear the CSV data.
		 * @return A boolean indicating if the update was succesful
		 * (true) or not (false).
		 */
		bool setRows(const QList<QStringList> &lRows);

		/**
		 * Sets the values for the given row in the CSV.
		 * @param iRow Integer with the row to update. The row is
		 * counted from 0 to rowsCount()-1.
		 * @param lRow A QStringList with the values of the
		 * rows. It must have the same number of values as in
		 * columnsCount().
		 * @return A boolean indicating if the update was succesful
		 * (true) or not (false).
		 */
		bool setRow(const int iRow, const QStringList &lRow);

		/**
		 * Adds the values for a new row in the CSV.
		 * @param lRow A QStringList with the values of the
		 * new row. It must have the same number of values as in
		 * columnsCount().
		 * @return A boolean indicating if the update was succesful
		 * (true) or not (false).
		 */
		bool addRow(const QStringList &lRow);

	private:
		
		/** Header names in the CSV. */
		QStringList m_lHeader;

		/** Rows of data in the CSV. */
		QList<QStringList> m_lRows;

		/** Character used for separating values in the CSV file. */
		QChar m_cSeparator;

		/** Number of columns in the CSV loaded. */
		int m_iColumns;
	};
}

#endif // CSVFILE_H