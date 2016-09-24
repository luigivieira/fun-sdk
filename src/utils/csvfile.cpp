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

#include "csvfile.h"
#include <QTextStream>

// +-----------------------------------------------------------
fsdk::CSVFile::CSVFile(const int iColumns, const QChar cSeparator): QFile()
{
	m_cSeparator = cSeparator;
	m_iColumns = iColumns;
}

// +-----------------------------------------------------------
fsdk::CSVFile::CSVFile(const QString &sName, const int iColumns, const QChar cSeparator): QFile(sName)
{
	m_cSeparator = cSeparator;
	m_iColumns = iColumns;
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::load(const bool bHasHeader)
{
	if(!open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	QTextStream oReader(this);

	int iColumns = -1;

	QString sRow;
	QStringList lHeader, lRow;
	QList<QStringList> lRows;

	if(bHasHeader)
	{
		sRow = oReader.readLine();
		lHeader = sRow.split(m_cSeparator);
		iColumns = lHeader.count();
	}

	while(!oReader.atEnd())
	{
		sRow = oReader.readLine();
		if(sRow.isEmpty()) // Ignore blank rows
			continue;

		lRow = sRow.split(m_cSeparator);

		if(iColumns == -1)
			iColumns = lRow.count();
		
		// Guarantee that header and each row have the
		// exactly same number of elements
		if(lRow.count() != iColumns)
		{
			close();
			return false;
		}

		lRows.push_back(lRow);
	}

	close();

	m_lHeader = lHeader;
	m_lRows = lRows;
	m_iColumns = iColumns;
	return true;
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::loadFromFile(const QString &sName, const bool bHasHeader)
{
	if(isOpen())
		close();
	setFileName(sName);
	return load(bHasHeader);
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::save()
{
	if(!open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	QTextStream oWriter(this);

	if(m_lHeader.count() > 0)
		oWriter << m_lHeader.join(m_cSeparator) << endl;

	foreach(QStringList lRow, m_lRows)
		oWriter << lRow.join(m_cSeparator) << endl;
	
	close();
	return true;
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::saveToFile(const QString &sName)
{
	if(isOpen())
		close();
	setFileName(sName);
	return save();
}

// +-----------------------------------------------------------
QChar fsdk::CSVFile::separator() const
{
	return m_cSeparator;
}

// +-----------------------------------------------------------
void fsdk::CSVFile::setSeparator(const QChar cSeparator)
{
	m_cSeparator = cSeparator;
}

// +-----------------------------------------------------------
int fsdk::CSVFile::columnsCount() const
{
	return m_iColumns;
}

// +-----------------------------------------------------------
int fsdk::CSVFile::rowsCount() const
{
	return m_lRows.count();
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::hasHeader() const
{
	return m_lHeader.count() != 0;
}

// +-----------------------------------------------------------
QStringList fsdk::CSVFile::header() const
{
	return m_lHeader;
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::setHeader(const QStringList &lHeader)
{
	if(lHeader.count() == 0 || lHeader.count() == m_iColumns)
	{
		m_lHeader = lHeader;
		return true;
	}
	else
		return false;
}

// +-----------------------------------------------------------
QList<QStringList> fsdk::CSVFile::rows() const
{
	return m_lRows;
}

// +-----------------------------------------------------------
QStringList fsdk::CSVFile::row(const int iRow) const
{
	if(iRow >= 0 && iRow < m_lRows.size())
		return m_lRows[iRow];
	else
		return QStringList();
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::setRows(const QList<QStringList> &lRows)
{
	int iColumns = -1;

	// Check if all new rows have the same number of columns
	foreach(QStringList lRow, lRows)
	{
		if(iColumns == -1)
			iColumns = lRow.count();

		if(lRow.count() != iColumns)
			return false;
	}

	if(lRows.count() == 0 || iColumns == m_iColumns)
	{
		m_lRows = lRows;
		return true;
	}
	else
		return false;
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::setRow(const int iRow, const QStringList &lRow)
{
	if(iRow >= 0 && iRow < m_lRows.count())
	{
		if(lRow.count() == m_iColumns)
		{
			m_lRows[iRow] = lRow;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}