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
#include <QRegularExpression>

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

	// Use a regular expression to consider exactly 1 ocurrence
	// of the text delimiter, in order to allow double ocurrences
	// as "escape" ocurrences (for instance, considering the character
	// '"' as the delimiter, the contents of field "aa""aa" are: [aa""aa]).
	QRegularExpression oSingleDelimiter(QString("(?<!%1)%2(?!%3)").arg(m_sTextDelimiter, m_sTextDelimiter, m_sTextDelimiter));

	QStringList lLine;
	QString sLine, sField;
	int iPosDel, iPosSep;
	
	while(!oReader.atEnd())
	{
		sLine = oReader.readLine();
	
		// Since the call to oReader.readLine strips all "\n" from the input,
		// in case the line is empty but we are processing a field, this line
		// can not be ignored (because it belongs to the contents of the multiline
		// field!).
		if(sLine.isEmpty() && eScope == InField)
			sLine = QString("\n");

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
						// Get the position of the ending delimiter after the starting one
						iPosDel = sLine.indexOf(oSingleDelimiter, m_sTextDelimiter.length());

						// If the ending delimiter is not found, it means
						// that this is a multiline field (i.e. that includes new
						// line characters). So, save its contents and keep reading
						// it in next line(s).
						if(iPosDel == -1)
						{
							eScope = InField;
							sField = sLine.mid(m_sTextDelimiter.length()) + QString("\n");
							sLine = "";
						}

						// Ending delimiter found: process just that field until the next
						// field separator (observation: any data between the ending delimiter
						// and the next field separator or end-of-line are simply ignored, since
						// this is a format error).
						else
						{
							lLine.append(sLine.mid(m_sTextDelimiter.length(), iPosDel - m_sTextDelimiter.length()));
							iPosSep = sLine.indexOf(m_sFieldSeparator, iPosDel + m_sTextDelimiter.length());
							if(iPosSep == -1)
								sLine = "";
							else
								sLine = sLine.mid(iPosSep + m_sFieldSeparator.length());
						}
					}

					// The next field in line does not start with text delimiter.
					// So, process the field normally: that is, find the next
					// field separator, extract it and add to the list
					else
					{
						// Get the position of the next field separator
						iPosSep = sLine.indexOf(m_sFieldSeparator);

						// If no next separator, this is the last field in line
						if(iPosSep == -1)
						{
							lLine.append(sLine);
							sLine = "";
						}

						// Otherwise, extract just that field
						else
						{
							lLine.append(sLine.mid(0, iPosSep));
							sLine = sLine.mid(iPosSep + m_sFieldSeparator.length());
						}
					}
					break;

				// We are reading the contents of a multiline field
				// (i.e. a field delimited by the configured text delimiter)
				case InField:

					// Get the position of the ending delimiter in the new line
					iPosDel = sLine.indexOf(oSingleDelimiter);

					// If the ending delimiter is not found, it means
					// that the multiline field continues with more new lines.
					// So, save its contents and keep on reading!
					if(iPosDel == -1)
					{
						sField += sLine;
						if(sLine != QString("\n"))
							sField += QString("\n");
						sLine = "";
					}

					// The ending delimiter has been found!
					// Add the field contents to the return list and
					// go back to the line processing scope. Again,
					// any content existing between the ending delimiter
					// and the next field separator or end-of-line is ignored
					// (format error).
					else
					{
						sField += sLine.mid(0, iPosDel);
						lLine.append(sField);
						sField = "";

						iPosSep = sLine.indexOf(m_sFieldSeparator, iPosDel + m_sTextDelimiter.length());
						if(iPosSep == -1)
							sLine = "";
						else
							sLine = sLine.mid(iPosSep + m_sFieldSeparator.length());

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

		// Enclose the header with the text delimiter if it is multiline or
		// contains the field separator or text delimiter themselves.
		if(sHeaderLabel.contains("\n") || sHeaderLabel.contains(m_sFieldSeparator) || sHeaderLabel.contains(m_sTextDelimiter))
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

			// Enclose the field with the text delimiter if it is multiline or
			// contains the field separator or text delimiter themselves.
			if(sField.contains("\n") || sField.contains(m_sFieldSeparator) || sField.contains(m_sTextDelimiter))
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
QList<QStringList>& fsdk::CSVFile::lines()
{
	return m_lLines;
}

// +-----------------------------------------------------------
void fsdk::CSVFile::addLine(const QStringList &lLine)
{
	m_lLines.append(lLine);
}