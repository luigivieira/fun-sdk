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

// +-----------------------------------------------------------
fsdk::Session::Session(QObject *pParent) :
	QObject(pParent)
{
	m_sPlayerFileName = "";
	m_sGameplayFileName = "";
	m_sLandmarksFileName = "";
	m_ePlayerStatus = NotDefined;
	m_eGameplayStatus = NotDefined;
}

// +-----------------------------------------------------------
QString fsdk::Session::playerFileName() const
{
	return m_sPlayerFileName;
}

// +-----------------------------------------------------------
void fsdk::Session::setPlayerFileName(const QString &sFileName)
{
	if(m_ePlayerStatus == Ok)
		m_oPlayerVideo.release();

	m_sPlayerFileName = sFileName;
	if(m_sPlayerFileName.isEmpty())
		m_ePlayerStatus = NotDefined;
	else if(!m_oPlayerVideo.open(m_sPlayerFileName.toStdString()))
		m_ePlayerStatus = ReadError;
	else
		m_ePlayerStatus = Ok;
}

// +-----------------------------------------------------------
QString fsdk::Session::gameplayFileName() const
{
	return m_sGameplayFileName;
}

// +-----------------------------------------------------------
void fsdk::Session::setGameplayFileName(const QString &sFileName)
{
	if(m_eGameplayStatus == Ok)
		m_oGameplayVideo.release();

	m_sGameplayFileName = sFileName;
	if(m_sGameplayFileName.isEmpty())
		m_eGameplayStatus = NotDefined;
	else if(!m_oGameplayVideo.open(m_sGameplayFileName.toStdString()))
		m_eGameplayStatus = ReadError;
	else
		m_eGameplayStatus = Ok;
}

// +-----------------------------------------------------------
void fsdk::Session::save(const QString &sFileName)
{

}

// +-----------------------------------------------------------
void fsdk::Session::load(const QString &sFileName)
{

}
