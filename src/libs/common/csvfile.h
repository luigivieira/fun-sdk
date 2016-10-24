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

#include "libexport.h"
#include <QFile>
#include <QTextCodec>
#include <QList>
#include <QStringList>
#include <QTextStream>

namespace fsdk
{
	/**
	 * Allows reading and writing comma-separated-value (CSV) files
	 * according to the RFC 4180 (https://tools.ietf.org/pdf/rfc4180.pdf).
	 */
	class SHARED_LIB_EXPORT CSVFile: public QFile
	{
	public:
		/**
		 * Default constructor.
		 * @param sFilename QString with the name of the CSV file. Optional (default is "").
		 * @param sFieldSeparator QString with the field separator. Optional (default is ",").
		 * @param sTextDelimiter QString with the text delimiter. Optional (default is "\"").
		 * @param pCodec Pointer to a QTextCodec used when reading/writting the CSV contents.
		 * Optional (default is the UTF-8 codec).
		 */
		CSVFile(const QString &sFilename = "", const QString &sFieldSeparator = ",", const QString &sTextDelimiter = "\"", QTextCodec *pCodec = QTextCodec::codecForName("UTF-8"));

		/**
		 * Reads the contents of the CSV file specified in fileName().
		 * @param bHeader Boolean indicating if the CSV has a header (true)
		 * or not (false).
		 * @return Boolean indicating if the reading was successful (true)
		 * or not (false).
		 */
		bool read(const bool bHeader = true);

		/**
		 * Reads the contents of the given CSV filename.
		 * @param sFilename QString with the name of the file to read from.
		 * @param bHeader Boolean indicating if the CSV has a header (true)
		 * or not (false).
		 * @return Boolean indicating if the reading was successful (true)
		 * or not (false).
		 */
		bool read(const QString &sFilename, const bool bHeader = true);

		/**
		 * Writes the contents of the CSV to the file specified in fileName().
		 * @return Boolean indicating if the writting was successful (true)
		 * or not (false).
		 */
		bool write();

		/**
		 * Writes the contents of the CSV to the given filename.
		 * @param sFilename QString with the name of the file to write to.
		 * @return Boolean indicating if the writting was successful (true)
		 * or not (false).
		 */
		bool write(const QString &sFilename);

		/**
		 * Indicates if the CSV has a header.
		 * @return Boolean indicating if the CSV has a
		 * header (true) or not (false).
		 */
		bool hasHeader() const;

		/**
		 * Gets the header line of this CSV.
		 * @return Modifiable reference to the QStringList with the header.
		 */
		QStringList& header();

		/**
		 * Gets the given line of this CSV.
		 * @param iLine Integer with the line in range [0, lines()-1].
		 * The value of this argument MUST be in that range, otherwise
		 * an access error will ocurr.
		 * @return Modifiable reference to the QStringList with the data
		 * in the requested line.
		 */
		QStringList& line(const int iLine);

		/**
		 * Gets all the lines of this CSV (excluding the header, if read
		 * or created as such).
		 * @return Modifiable reference to the QList of QStringList's with
		 * all lines of data.
		 */
		QList<QStringList>& lines();

		/**
		 * Adds a new line of data to the CSV. To add or change the data
		 * in the header, use the header() method.
		 * @param lLine QStringList with the data to add to the CSV.
		 */
		void addLine(const QStringList &lLine);

	protected:

		/**
		 * Reads a record line from the given text stream, automatically
		 * handling multiline fields (i.e. using the text delimiter).
		 * @param oReader Reference to the QTextStream used to read the file.
		 * @return QStringList with the contents of the fields read from the
		 * file in a "logical" line (that may include multiple real lines
		 * in the file due to multiline fields).
		 */
		QStringList readLine(QTextStream &oReader);

	private:
		
		/** Header names in the CSV. */
		QStringList m_lHeader;

		/** List of records in the CSV. */
		QList<QStringList> m_lLines;

		/** Separator used to differentiate fields in the CSV. */
		QString m_sFieldSeparator;

		/** Texte delimiter used to enclose strings in the CSV. */
		QString m_sTextDelimiter;

		/** Codec used to read/write the contents of the CSV. */
		QTextCodec *m_pCodec;
	};
}

#endif // CSVFILE_H