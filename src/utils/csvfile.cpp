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

	QString sLine;
	QStringList lHeader, lLine;
	QList<QStringList> lValues;

	if(bHasHeader)
	{
		sLine = oReader.readLine();
		lHeader = sLine.split(m_cSeparator);
		iColumns = lHeader.count();
	}

	while(!oReader.atEnd())
	{
		sLine = oReader.readLine();
		lLine = sLine.split(m_cSeparator);

		if(iColumns == -1)
			iColumns = lLine.count();
		
		// Guarantee that header and each row have the
		// exactly same number of elements
		if(lLine.count() != iColumns)
		{
			close();
			return false;
		}

		lValues.push_back(lLine);
	}

	close();

	m_lHeader = lHeader;
	m_lValues = lValues;
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
int fsdk::CSVFile::columns() const
{
	return m_iColumns;
}

// +-----------------------------------------------------------
int fsdk::CSVFile::rows() const
{
	return m_lValues.count();
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
QList<QStringList> fsdk::CSVFile::values() const
{
	return m_lValues;
}

// +-----------------------------------------------------------
QStringList fsdk::CSVFile::rowValues(const int iRow) const
{
	if(iRow >= 0 && iRow < m_lValues.size())
		return m_lValues[iRow];
	else
		return QStringList();
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::setValues(const QList<QStringList> &lValues)
{
	int iColumns = -1;

	// Check if all new rows have the same number of columns
	foreach(QStringList lRow, lValues)
	{
		if(iColumns == -1)
			iColumns = lRow.count();

		if(lRow.count() != iColumns)
			return false;
	}

	if(lValues.count() == 0 || iColumns == m_iColumns)
	{
		m_lValues = lValues;
		return true;
	}
	else
		return false;
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::setRowValues(const int iRow, const QStringList &lValues)
{
	if(iRow >= 0 && iRow < m_lValues.count())
	{
		if(lValues.count() == m_iColumns)
		{
			m_lValues[iRow] = lValues;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}