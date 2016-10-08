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

#include "application.h"
#include "version.h"
#include <typeinfo>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileInfo>
#include <iostream>

#if defined(_DEBUG) && defined(_WIN32)
	#include "Windows.h"
#endif

// Constants for the settings
#define SETTINGS_KEY_LOGLEVEL "logLevel"

// +-----------------------------------------------------------
fsdk::Application::Application(int& argc, char** argv, const QString &sOrgName, const QString &sOrgDomain, const QString &sAppName, const QString &sAppVersion, const bool bUseSettings) :
#ifdef CONSOLE
	QCoreApplication(argc, argv)
#else
	QApplication(argc, argv)
#endif
{
	m_eLogLevel = Fatal; // By default, only fatal messages are logged.
	setOrganizationName(sOrgName);
	setOrganizationDomain(sOrgDomain);
	setApplicationName(sAppName);
	setApplicationVersion(sAppVersion);

	// Setup message logging
#ifndef CONSOLE
	QFileInfo oAppFile = QFileInfo(applicationFilePath());
	QString sLogFile = QString("%1/%2.log").arg(applicationDirPath()).arg(oAppFile.baseName());

	m_pLogFile = new QFile(sLogFile);
	if(!m_pLogFile->open(QIODevice::Append | QIODevice::Text))
		qFatal("Error opening log file %s for writing", qPrintable(sLogFile));
	m_pLogStream = new QTextStream(m_pLogFile);
#endif

#if defined(_DEBUG)
	qSetMessagePattern("%{time yyyy.MM.dd h:mm:ss.zzz} [%{type} (%{function} in %{file}:%{line})]: %{message}");
#else
	qSetMessagePattern("%{time yyyy.MM.dd h:mm:ss.zzz} [%{type}]: %{message}");
#endif
	qInstallMessageHandler(&fsdk::Application::handleLogOutput);

	// Read configuration if required
	if(bUseSettings)
	{
		m_pSettings = new QSettings(QSettings::UserScope, organizationName(), organizationDomain());
		m_pSettings->beginGroup(applicationName()); // base group for all settings

		setLogLevel(static_cast<LogLevel>(m_pSettings->value(SETTINGS_KEY_LOGLEVEL, Fatal).toInt()));
	}
	else
	{
		m_pSettings = NULL;
		setLogLevel(Fatal);
	}
}

// +-----------------------------------------------------------
fsdk::Application::~Application()
{
	if(m_pSettings)
		delete m_pSettings;

	qInstallMessageHandler(0);
	
#ifndef CONSOLE
	m_pLogStream->flush();
	delete m_pLogStream;

	m_pLogFile->close();
	delete m_pLogFile;
#endif
}

// +-----------------------------------------------------------
QSettings* fsdk::Application::settings()
{
	return m_pSettings;
}

// +-----------------------------------------------------------
int fsdk::Application::exec()
{
	qDebug("%s (v%s) started.", qPrintable(applicationName()), qPrintable(applicationVersion()));
	qDebug("Running from %s", qPrintable(applicationFilePath()));

	int iRet = QCoreApplication::exec();

	if (iRet == 0)
		qDebug("%s (v%s) terminated successfully.", qPrintable(applicationName()), qPrintable(applicationVersion()));
	else
		qDebug("%s (v%s) terminated with error code: %d.", qPrintable(applicationName()), qPrintable(applicationVersion()), iRet);
	return iRet;
}

// +-----------------------------------------------------------
fsdk::Application::LogLevel fsdk::Application::logLevel() const
{
	return m_eLogLevel;
}

// +-----------------------------------------------------------
void fsdk::Application::setLogLevel(Application::LogLevel eLevel)
{
	if(eLevel < Fatal)
		m_eLogLevel = Fatal;
	else if(eLevel > Debug)
		m_eLogLevel = Debug;
	else
		m_eLogLevel = eLevel;

	if(m_pSettings)
		m_pSettings->setValue(SETTINGS_KEY_LOGLEVEL, m_eLogLevel);
}

// +-----------------------------------------------------------
bool fsdk::Application::notify(QObject* pReceiver, QEvent* pEvent)
{
    try
	{
#ifdef CONSOLE
		return QCoreApplication::notify(pReceiver, pEvent);
#else
		return QApplication::notify(pReceiver, pEvent);
#endif
    }
    catch (std::exception &e)
	{
        qFatal("The exception %s ocurred while sending event [%s] to object [%s] (%s)",
            e.what(), typeid(*pEvent).name(), qPrintable(pReceiver->objectName()), typeid(*pReceiver).name());
    }
    catch (...)
	{
        qFatal("An exception ocurred while sending event [%s] to object [%s] (%s)",
            typeid(*pEvent).name(), qPrintable(pReceiver->objectName()), typeid(*pReceiver).name());
    }

    return false;
}

// +-----------------------------------------------------------
void fsdk::Application::handleLogOutput(QtMsgType eType, const QMessageLogContext &oContext, const QString &sMessage)
{
	// Convert Qt's message type to a log level
	LogLevel eLevel = static_cast<LogLevel>(eType == QtDebugMsg ? 4 : (eType == QtInfoMsg ? 3 : std::abs(eType - QtFatalMsg)));

	// Do not log the message if its level is bigger then the maximum configured
	LogLevel eMax = static_cast<Application*>(qApp)->m_eLogLevel;
	if(eLevel > static_cast<Application*>(qApp)->m_eLogLevel)
		return;

	// Format the message according to the value defined by the call to qSetMessagePattern
	// or to the value defined in QT_MESSAGE_PATTERN environment variable
	// (the environment variable has precedence - see documentation to qDebug)
	QString sMsg = qFormatLogMessage(eType, oContext, sMessage);

	// ONLY IF DEBUG AND USING VISUAL STUDIO: print in VS output's window
#if defined(_DEBUG) && defined(_WIN32)
	OutputDebugString(qPrintable(sMsg));
#endif

	// Output the message properly formatted according to the type of application
	// (i.e. CONSOLE vs GUI)
#ifdef CONSOLE
	if(eType == QtFatalMsg || eType == QtCriticalMsg)
		std::cerr << sMsg.toStdString() << std::endl;
	else
		std::cout << sMsg.toStdString() << std::endl;
#else
	*(static_cast<Application*>(qApp)->m_pLogStream) << sMsg << endl;
	static_cast<Application*>(qApp)->m_pLogStream->flush();

	// If the message is fatal, output it also to stderr and present a message box to the user
	if (eType == QtFatalMsg)
	{
		std::cerr << qPrintable(sMsg) << std::endl;
		QApplication::beep();
		QMessageBox::critical(NULL, qApp->translate("Application", "Runtime Error"), qApp->translate("Application", "A fatal error ocurred and the application must terminate. Please check the log file for details."), QMessageBox::Ok);
	}
#endif
}