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

#include "session.h"
#include "conversion.h"
#include <QDebug>
#include <QDomDocument>
#include <QApplication>

// +-----------------------------------------------------------
fsdk::Session::Session(QObject *pParent) :
	QObject(pParent)
{
	m_sPlayerFileName = "";
	m_sGameplayFileName = "";
	m_sLandmarksFileName = "";
	m_ePlayerStatus = NotDefined;
	m_eGameplayStatus = NotDefined;
	m_bModified = false;
}

// +-----------------------------------------------------------
QString fsdk::Session::playerFileName() const
{
	return m_sPlayerFileName;
}

// +-----------------------------------------------------------
void fsdk::Session::setPlayerFileName(const QString &sFileName)
{
	if(m_sPlayerFileName == sFileName)
		return;

	qDebug().noquote() << "Setting player video to: " << sFileName;
	m_sPlayerFileName = sFileName;
	setModified(true);
}

// +-----------------------------------------------------------
QString fsdk::Session::gameplayFileName() const
{
	return m_sGameplayFileName;
}

// +-----------------------------------------------------------
void fsdk::Session::setGameplayFileName(const QString &sFileName)
{
	if(m_sGameplayFileName == sFileName)
		return;

	qDebug().noquote() << "Setting gameplay video to: " << sFileName;
	m_sGameplayFileName = sFileName;
	setModified(true);
}

// +-----------------------------------------------------------
QString fsdk::Session::landmarksFileName() const
{
	return m_sLandmarksFileName;
}

// +-----------------------------------------------------------
void fsdk::Session::setLandmarksFileName(const QString &sFileName)
{
	if(m_sLandmarksFileName == sFileName)
		return;

	qDebug().noquote() << "Setting landmarks to: " << sFileName;
	m_sLandmarksFileName = sFileName;
	setModified(true);
}

// +-----------------------------------------------------------
void fsdk::Session::clear()
{
	m_sSessionFileName = "";
	m_sPlayerFileName = "";
	m_sGameplayFileName = "";
	m_sLandmarksFileName = "";
	setModified(false);
}

// +-----------------------------------------------------------
bool fsdk::Session::save(const QString &sFileName)
{
	/******************************************************
	 * Create the xml document
	 ******************************************************/
	QDomDocument oDoc;

	// Processing instruction
	QDomProcessingInstruction oInstr = oDoc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
	oDoc.appendChild(oInstr);

	// Root node
	QDomElement oRoot = oDoc.createElementNS("https://github.com/luigivieira/fun-sdk", "Session");
	oDoc.appendChild(oRoot);
	oRoot.setAttribute("version", qApp->applicationVersion());

	// Video files
	QDomElement oVideos = oDoc.createElement("Videos");
	oRoot.appendChild(oVideos);

	// Player video file
	QDomElement oPlayer = oDoc.createElement("Player");
	oVideos.appendChild(oPlayer);
	oPlayer.setAttribute("fileName", m_sPlayerFileName);

	// Gameplay video file
	QDomElement oGameplay = oDoc.createElement("Gameplay");
	oVideos.appendChild(oGameplay);
	oGameplay.setAttribute("fileName", m_sGameplayFileName);

	// Annotation files
	QDomElement oAnnotations = oDoc.createElement("Annotations");
	oRoot.appendChild(oAnnotations);

	// Landmarks file
	QDomElement oLandmarks = oDoc.createElement("Landmarks");
	oAnnotations.appendChild(oLandmarks);
	oLandmarks.setAttribute("fileName", m_sLandmarksFileName);

	/******************************************************
	 * Save the file
	 ******************************************************/
	QFile oFile(sFileName);
	if(!oFile.open(QFile::WriteOnly | QFile::Truncate))
	{
		qWarning().noquote() << tr("Error writing file %1").arg(sFileName);
		return false;
	}

	QTextStream oData(&oFile);
	oDoc.save(oData, 4);
	oFile.close();

	m_sSessionFileName = sFileName;
	setModified(false);
	return true;
}

// +-----------------------------------------------------------
bool fsdk::Session::load(const QString &sFileName)
{
	/******************************************************
	 * Open and read the file
	 ******************************************************/
	QFile oFile(sFileName);
	if(!oFile.open(QFile::ReadOnly))
	{
		qWarning().noquote() << tr("Error reading file %1").arg(sFileName);
		return false;
	}

	QTextStream oData(&oFile);
	QString sData = oData.readAll();
	oFile.close();

	/******************************************************
	 * Parse the xml document
	 ******************************************************/
	QDomDocument oDoc;
	QString sError;
	int iLine, iColumn;
	if(!oDoc.setContent(sData, true, &sError, &iLine, &iColumn))
	{
		qWarning().noquote() << tr("Error reading file %1: %2, line %3, column %4").arg(sFileName, sError, QString::number(iLine), QString::number(iColumn));
		return false;
	}

	// Root node
	QDomElement oRoot = oDoc.firstChildElement("Session");

	if(oRoot.isNull())
	{
		qWarning().noquote() << tr("Error reading file %1: the node '%2' does not exist").arg(sFileName, "Session");
		return false;
	}

	// To be used in the future, for version backward compatibility
	// QString sVersion = oRoot.attribute("version", "");
	
	// Video files
	QDomElement oVideos = oRoot.firstChildElement("Videos");
	if(oVideos.isNull())
	{
		qWarning().noquote() << tr("Error reading file %1: the node '%2' does not exist").arg(sFileName, "Videos");
		return false;
	}

	// Player video file
	QDomElement oPlayer = oVideos.firstChildElement("Player");
	if(oPlayer.isNull())
	{
		qWarning().noquote() << tr("Error reading file %1: the node '%2' does not exist").arg(sFileName, "Player");
		return false;
	}
	QString sPlayerFileName = oPlayer.attribute("fileName", "");

	// Gameplay video file
	QDomElement oGameplay = oVideos.firstChildElement("Gameplay");
	if(oGameplay.isNull())
	{
		qWarning().noquote() << tr("Error reading file %1: the node '%2' does not exist").arg(sFileName, "Gameplay");
		return false;
	}
	QString sGameplayFileName = oGameplay.attribute("fileName", "");

	// Annotation files
	QDomElement oAnnotations = oRoot.firstChildElement("Annotations");
	if(oAnnotations.isNull())
	{
		qWarning().noquote() << tr("Error reading file %1: the node '%2' does not exist").arg(sFileName, "Annotations");
		return false;
	}

	// Landmarks file
	QDomElement oLandmarks = oAnnotations.firstChildElement("Landmarks");
	if(oLandmarks.isNull())
	{
		qWarning().noquote() << tr("Error reading file %1: the node '%2' does not exist").arg(sFileName, "Landmarks");
		return false;
	}
	QString sLandmarksFileName = oLandmarks.attribute("fileName", "");

	m_sSessionFileName = sFileName;
	m_sPlayerFileName = sPlayerFileName;
	m_sGameplayFileName = sGameplayFileName;
	m_sLandmarksFileName = sLandmarksFileName;
	setModified(false);

	return true;
}

// +-----------------------------------------------------------
bool fsdk::Session::isModified() const
{
	return m_bModified;
}

// +-----------------------------------------------------------
void fsdk::Session::setModified(const bool bModified)
{
	m_bModified = bModified;
	emit sessionChanged(m_sSessionFileName, m_sPlayerFileName, m_sGameplayFileName, m_sLandmarksFileName);
}

// +-----------------------------------------------------------
QString fsdk::Session::sessionFileName() const
{
	return m_sSessionFileName;
}