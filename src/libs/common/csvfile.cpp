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

#include "csvfile.h"
#include <QDebug>

// +-----------------------------------------------------------
fsdk::CSVFile::CSVFile(const QString &sFilename, const QString &sFieldSeparator, const QString &sTextDelimiter, QTextCodec *pCodec):
	QFile()
{
	if(!sFilename.isEmpty())
		setFileName(sFilename);

	m_sFieldSeparator = sFieldSeparator;
	m_sTextDelimiter = sTextDelimiter;
	m_pCodec = pCodec;
}

// +-----------------------------------------------------------
QStringList fsdk::CSVFile::readLine(QTextStream &oReader)
{
	// This is the scope of the current processing in the inner loop
	enum Scope { InLine, InField };
	Scope eScope = InLine;

	QStringList lLine;
	QString sLine, sField;
	int iPos;
	
	while(!oReader.atEnd())
	{
		sLine = oReader.readLine();
		sLine += QString("\n"); // QTextStream strips out the new line, needed in this algorithm
	
		while(!sLine.isEmpty())
		{
			switch(eScope)
			{
				// We are reading fields in the line
				case InLine:

					// If the next field in line starts with a text delimiter,
					// handle its reading as a text field
					if(sLine.startsWith(m_sTextDelimiter))
					{
						// Get the position of the second delimiter after the first one
						iPos = sLine.indexOf(m_sTextDelimiter, m_sTextDelimiter.length());

						// If the second delimiter is not found, it means
						// that this is a multiline field (i.e. that includes new
						// line characters). So, save its contents and keep reading
						// it in next line(s).
						if(iPos == -1)
						{
							eScope = InField;
							sField = sLine.mid(m_sTextDelimiter.length());
							sLine = "";
						}
					}

					// No field starting with text delimiter in this line,
					// so process fields normally: that is, split the line
					// based on the field separator and add it to the
					// return list.
					else
					{
						lLine.append(sLine.replace("\n", "").split(m_sFieldSeparator));
						sLine = "";
					}
					break;

				// We are reading the contents of a multiline field
				// (i.e. a field delimited by the configured text delimiter)
				case InField:

					// Get the position of the second delimiter in the new line
					iPos = sLine.indexOf(m_sTextDelimiter);

					// If the second delimiter is not found, it means
					// that the multiline field continues with more new lines.
					// So, save its contents and keep on reading!
					if(iPos == -1)
					{
						sField += sLine.mid(m_sTextDelimiter.length());
						sLine = "";
					}

					// The second delimiter has been found!
					// Add the field contents to the return list and
					// go back to the line processing scope to keep processing.
					else
					{
						sField += sLine.mid(0, iPos);
						sLine = sLine.mid(iPos + 1);
						lLine.append(sField);
						sField = "";
						eScope = InLine;
					}
					break;				
			}
		}

		// If this point is reached in the InLine scope (sLine empty),
		// it means that the whole "line" (including multiline fields)
		// has already been read. So just break the processing and return
		// the line contents!
		if(eScope == InLine)
			break;
	}

	return lLine;
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::read(const bool bHeader)
{
	if(!open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug().noquote() << QString("Error opening file %1 for reading").arg(fileName());
		return false;
	}

	QTextStream oReader(this);
	oReader.setCodec(m_pCodec);

	QStringList lHeader;
	QList<QStringList> lLines;

	while(!oReader.atEnd())
	{
		if(bHeader && lHeader.isEmpty())
			lHeader = readLine(oReader);
		else
			lLines.append(readLine(oReader));
	}

	close();

	m_lHeader = lHeader;
	m_lLines = lLines;
	return true;
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::read(const QString &sFilename, const bool bHeader)
{
	setFileName(sFilename);
	return read(bHeader);
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::write()
{
	if(!open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug().noquote() << QString("Error opening file %1 for writing").arg(fileName());
		return false;
	}

	QTextStream oWriter(this);

	for(int i = 0; i < m_lHeader.count(); i++)
	{
		QString sHeaderLabel = m_lHeader[i];
		if(sHeaderLabel.contains(m_sFieldSeparator) || sHeaderLabel.contains("\n"))
			oWriter << m_sTextDelimiter << sHeaderLabel << m_sTextDelimiter;
		else
			oWriter << sHeaderLabel;
		if(i < m_lHeader.count() - 1)
			oWriter << m_sFieldSeparator;
		else
			oWriter << endl;
	}
	
	foreach(QStringList lLine, m_lLines)
	{
		for(int i = 0; i < lLine.count(); i++)
		{
			QString sField = lLine[i];
			if(sField.contains(m_sFieldSeparator) || sField.contains("\n"))
				oWriter << m_sTextDelimiter << sField << m_sTextDelimiter;
			else
				oWriter << sField;
			if(i < lLine.count() - 1)
				oWriter << m_sFieldSeparator;
			else
				oWriter << endl;
		}
	}
	
	close();
	return true;
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::write(const QString &sFilename)
{
	setFileName(sFilename);
	return write();
}

// +-----------------------------------------------------------
bool fsdk::CSVFile::hasHeader() const
{
	return m_lHeader.count() != 0;
}

// +-----------------------------------------------------------
QStringList& fsdk::CSVFile::header()
{
	return m_lHeader;
}

// +-----------------------------------------------------------
QStringList& fsdk::CSVFile::line(const int iLine)
{
	return m_lLines[iLine];
}

// +-----------------------------------------------------------
void fsdk::CSVFile::addLine(const QStringList &lLine)
{
	m_lLines.append(lLine);
}