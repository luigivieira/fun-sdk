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

#include "application.h"
#include "version.h"
#include <typeinfo>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QTranslator>
#include <iostream>
#include <QTextStream>

using namespace std;

#define GROUP_MAIN "Main"
#define SETTING_DEFAULT_LANGUAGE "defaultLanguage"
#define SETTING_LOG_LEVEL "logLevel"
#define SETTING_DATA_PATH "dataPath"
#define SETTING_GAMEPLAY_TIME_LIMIT "gameplayTimeLimit"
#define SETTING_GAMEPLAY_REVIEW_SAMPLES "gameplayReviewSamples"
#define SETTING_GAMEPLAY_REVIEW_INTERVAL "gameplayReviewInterval"
#define SETTING_LAST_SUBJECT_ID "lastSubjectID"

// +-----------------------------------------------------------
gc::Application::Application(int& argc, char** argv): QApplication(argc, argv)
{
	m_bWaitingForVideoFiles = false;

	// Create/Open the log file
	m_eLogLevel = Fatal;
	QFileInfo oAppFile = QFileInfo(QCoreApplication::applicationFilePath());
	QString sLogFile = QString("%1%2%3.log").arg(QCoreApplication::applicationDirPath()).arg(QDir::separator()).arg(oAppFile.baseName());

#ifdef _DEBUG
	// Remove the existing log file if compiled in debug
	QFile::remove(sLogFile);
#endif

	m_oLogFile.open(qPrintable(sLogFile), ios::app);
	if (!m_oLogFile.is_open())
		qFatal("Error opening log file %s for writing", qPrintable(sLogFile));
	qInstallMessageHandler(&gc::Application::handleLogOutput);

	// Read the configuration file
	QString sIniFile = QString("%1%2configuration.ini").arg(QCoreApplication::applicationDirPath()).arg(QDir::separator());
	if (!QFileInfo::exists(sIniFile))
		qFatal("Configuration file %s does not exist", qPrintable(sIniFile));
	m_pSettings = new QSettings(sIniFile, QSettings::IniFormat);

	m_pSettings->beginGroup(GROUP_MAIN);
	m_eCurrentLanguage = (Language) m_pSettings->value(SETTING_DEFAULT_LANGUAGE, EN_UK).toInt();
	m_eLogLevel = (LogLevel) m_pSettings->value(SETTING_LOG_LEVEL, Fatal).toInt();
	m_iGameplayTimeLimit = m_pSettings->value(SETTING_GAMEPLAY_TIME_LIMIT, 60).toUInt();
	m_iGameplayReviewSamples = m_pSettings->value(SETTING_GAMEPLAY_REVIEW_SAMPLES, 10).toUInt();
	m_iGameplayReviewInterval = m_pSettings->value(SETTING_GAMEPLAY_REVIEW_INTERVAL, 30).toUInt();
	m_sDataPath = m_pSettings->value(SETTING_DATA_PATH, "").toString();
	m_iSubjectID = m_pSettings->value(SETTING_LAST_SUBJECT_ID, 0).toUInt();
	m_pSettings->endGroup();

	// Validate the configurations
	if (m_eCurrentLanguage < EN_UK)
		m_eCurrentLanguage = EN_UK;
	else if (m_eCurrentLanguage > PT_BR)
		m_eCurrentLanguage = PT_BR;

	if (m_eLogLevel < Fatal)
		m_eLogLevel = Fatal;
	else if (m_eLogLevel > Debug)
		m_eLogLevel = Debug;

	if (!m_sDataPath.length())
		qFatal("The group/key [%s/%s] is missing in the configuration file.", GROUP_MAIN, SETTING_DATA_PATH);
	if (!QDir(m_sDataPath).exists())
	{
		qDebug("Configured data path does not exist. Trying to create it...");
		if (!QDir().mkpath(m_sDataPath))
			qFatal("Data path %s does not exist and could not be created.", qPrintable(m_sDataPath));
	}

	// Load the style sheet file from resources
	QFile oFile(":/resources/stylesheet.css");
	if (oFile.open(QIODevice::ReadOnly | QIODevice::Text))
		m_sStyleSheet = oFile.readAll();
	else
		qFatal("Error reading style sheet from resources.");
	setStyleSheet(m_sStyleSheet);
	
	m_pCurrentTranslator = NULL;

	// Load the translations
	m_pPTBRTranslator = new QTranslator(this);
	if (!m_pPTBRTranslator->load(QLocale("pt"), ":/translations/pt-br.qm"))
		qFatal("Could not load the pt-br translation from resource");

	// Conclude the application start-up
	qInfo("GameCap (v%s) started.", GC_VERSION);
	qDebug("Running from %s", qPrintable(QCoreApplication::applicationFilePath()));
	qDebug() << "Configured log level is: " << m_eLogLevel;
	qDebug() << "Configured gameplay time limit is: " << m_iGameplayTimeLimit;
	qDebug() << "Configured gameplay review samples is: " << m_iGameplayReviewSamples;
	qDebug() << "Configured gameplay review interval is: " << m_iGameplayReviewInterval;
	qDebug() << "Configured data path is: " << m_sDataPath;

	// Prepares the access to the video recordings and the games
	m_pVideoCapturer = new VideoCapturer(this);
	m_pGamePlayer = new GamePlayer(this);

	connect(m_pGamePlayer, &GamePlayer::gameplayStarted, this, &Application::onGameplayStarted);
	connect(m_pGamePlayer, &GamePlayer::gameplayEnded, this, &Application::onGameplayEnded);

	connect(m_pVideoCapturer, &VideoCapturer::captureStarted, this, &Application::onCaptureStarted);
	connect(m_pVideoCapturer, &VideoCapturer::captureEnded, this, &Application::onCaptureEnded);

	connect(&m_oTimer, &QTimer::timeout, this, &Application::onTimeout);

	// Sets the default language
	setLanguage(m_eCurrentLanguage);
}

// +-----------------------------------------------------------
gc::Application::~Application()
{
	qInstallMessageHandler(0);
	// Update and close the log file
    m_oLogFile.flush();
	m_oLogFile.close();
}

// +-----------------------------------------------------------
uint gc::Application::getSubjectID() const
{
	return m_iSubjectID;
}

// +-----------------------------------------------------------
QString gc::Application::getSubjectDataFolder() const
{
	return QString().sprintf("%s%csubject_%03d%c", qPrintable(m_sDataPath), QDir::separator(), m_iSubjectID, QDir::separator());
}

// +-----------------------------------------------------------
void gc::Application::newSubject()
{
	m_iSubjectID++;
	QDir oDir(getSubjectDataFolder());
	if(!oDir.exists())
		oDir.mkdir(getSubjectDataFolder());

	setupGameData(m_iGameplayTimeLimit);
}

// +-----------------------------------------------------------
void gc::Application::rejectSubject()
{
	QDir oDir(getSubjectDataFolder());
	if(oDir.exists())
		oDir.removeRecursively();
	m_iSubjectID--;

	setupGameData(m_iGameplayTimeLimit);
}

// +-----------------------------------------------------------
void gc::Application::setupGameData(uint iGameplayTime)
{
	m_oGameplayData.setup(iGameplayTime, m_iGameplayReviewSamples, m_iGameplayReviewInterval);
}

// +-----------------------------------------------------------
void gc::Application::confirmSubject()
{
	m_oGameplayData.save(getSubjectDataFolder());
	m_pGamePlayer->selectNextGame();

	// Save the last subject processed
	m_pSettings->beginGroup(GROUP_MAIN);
	m_pSettings->setValue(SETTING_LAST_SUBJECT_ID, m_iSubjectID);
	m_pSettings->endGroup();
}

// +-----------------------------------------------------------
QSettings* gc::Application::getSettings()
{
	return m_pSettings;
}

// +-----------------------------------------------------------
gc::Application::Language gc::Application::getLanguage()
{
	return m_eCurrentLanguage;
}

// +-----------------------------------------------------------
void gc::Application::setLanguage(Language eLanguage)
{
	// Remove current translator (if any)
	if (m_pCurrentTranslator)
	{
		qApp->removeTranslator(m_pCurrentTranslator);
		m_pCurrentTranslator = NULL;
	}

	m_eCurrentLanguage = eLanguage;

	// Identify the translator according to the language
	switch (m_eCurrentLanguage)
	{
		case PT_BR:
			m_pCurrentTranslator = m_pPTBRTranslator;
			break;
	}

	// Istall the required translator and emit the change signal
	if (m_pCurrentTranslator)
		qApp->installTranslator(m_pCurrentTranslator);

	emit languageChanged(m_eCurrentLanguage);
}

// +-----------------------------------------------------------
gc::GamePlayer* gc::Application::gamePlayer()
{
	return m_pGamePlayer;
}

// +-----------------------------------------------------------
gc::VideoCapturer* gc::Application::videoCapturer()
{
	return m_pVideoCapturer;
}

// +-----------------------------------------------------------
int gc::Application::exec()
{
	int iRet = QApplication::exec();
	if (iRet == 0)
		qInfo("GameCap terminated successfully.");
	else
		qInfo("GameCap terminated with error code: %d", iRet);
	return iRet;
}

// +-----------------------------------------------------------
gc::Application::LogLevel gc::Application::getLogLevel()
{
	return m_eLogLevel;
}

// +-----------------------------------------------------------
void gc::Application::setLogLevel(Application::LogLevel eLevel)
{
	m_eLogLevel = eLevel;
}

// +-----------------------------------------------------------
bool gc::Application::notify(QObject* pReceiver, QEvent* pEvent)
{
    try
	{
		// Retransmit the event notification
        return QApplication::notify(pReceiver, pEvent);
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
void gc::Application::handleLogOutput(QtMsgType eType, const QMessageLogContext &oContext, const QString &sMsg)
{
	// Convert the qt message type to a log level
	LogLevel eLevel = (LogLevel) (eType == QtDebugMsg ? 4 : (eType == QtInfoMsg ? 3 : std::abs(eType - QtFatalMsg)));

	// Do not log the message if its level is bigger then the maximum configured.
	LogLevel eMax = static_cast<Application*>(qApp)->m_eLogLevel;
	if(eLevel > static_cast<Application*>(qApp)->m_eLogLevel)
		return;

	QString sNow = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

	QString sDebugInfo = " ";
#ifdef _DEBUG
	if(oContext.line != 0)
	{
		QString sSource = QFileInfo(QFile(oContext.file).fileName()).fileName();
		sDebugInfo = QString(" [%1:%2, %3] ").arg(sSource).arg(oContext.line).arg(oContext.function);
	}
#endif

	QString sLevel;
	switch (eType)
	{
		case QtDebugMsg:
			sLevel = "DEBUG: ";
			break;

		case QtInfoMsg:
			sLevel = "INFO: ";
			break;

		case QtWarningMsg:
			sLevel = "WARNING: ";
			break;

		case QtCriticalMsg:
			sLevel = "CRITICAL: ";
			break;

		case QtFatalMsg:
			sLevel = "FATAL: ";
			break;
	}

	static_cast<Application*>(qApp)->m_oLogFile << qPrintable(sNow) << qPrintable(sDebugInfo) << qPrintable(sLevel) << qPrintable(sMsg) << endl;
	static_cast<Application*>(qApp)->m_oLogFile.flush();

	if (eType == QtFatalMsg)
	{
		cerr << qPrintable(sNow) << qPrintable(sDebugInfo) << qPrintable(sLevel) << qPrintable(sMsg) << endl;

		QApplication::beep();
		QMessageBox::critical(NULL, qApp->translate("Main", "Runtime Error"), qApp->translate("Main", "A fatal exception ocurred and the application must be terminated. Please check the log file for details."), QMessageBox::Ok);
	}
}

// +-----------------------------------------------------------
void gc::Application::onTimeout()
{
	if(m_bWaitingForVideoFiles)
	{
		// Wait for the two videos to appear in the configured path,
		// so the amount of time configured for gameplay is recorded *exactly*
		// and the game can be started AFTER the OBS windows (in order to not
		// lose the focus)
		if(m_pVideoCapturer->getCapturedVideoFiles().size() == 2)
		{
			m_bWaitingForVideoFiles = false;
			m_oTimer.stop();
			m_pGamePlayer->startGameplay();
		}
	}
	else
	{
		m_iTimeRemaining--;
		if(m_iTimeRemaining <= 0)
			stopGameplay();
		else
			emit gameplayTimeRemaining(m_iTimeRemaining);
	}
}

// +-----------------------------------------------------------
void gc::Application::startGameplay()
{
#ifndef _DEBUG
	m_pMainWindow->setWindowFlags(m_pMainWindow->windowFlags() & ~Qt::WindowStaysOnTopHint);
	m_pMainWindow->setWindowFlags(m_pMainWindow->windowFlags() | Qt::WindowStaysOnBottomHint);
	m_pMainWindow->show();
#endif
	
	m_sGameplayFile = getSubjectDataFolder() + "gameplay.mp4";
	m_sPlayerFile = getSubjectDataFolder() + "player.mp4";
	m_bFailureSignalled = false;
	m_pVideoCapturer->startCapture(m_sGameplayFile, m_sPlayerFile);
}

// +-----------------------------------------------------------
QString gc::Application::getGameplayFile()
{
	return m_sGameplayFile;
}

// +-----------------------------------------------------------
void gc::Application::stopGameplay()
{
	m_pVideoCapturer->stopCapture();
	m_oTimer.stop();
	m_iTimeRemaining = 0;
}

// +-----------------------------------------------------------
void gc::Application::onGameplayStarted()
{
	m_bWaitingForVideoFiles = false;

	m_iTimeRemaining = m_iGameplayTimeLimit;
	emit gameplayTimeRemaining(m_iTimeRemaining);

	m_oTimer.setInterval(1000);
	m_oTimer.start();
}

// +-----------------------------------------------------------
void gc::Application::onGameplayEnded(GamePlayer::GameplayResult eResult)
{
	if(m_bFailureSignalled)
		return;

	switch(eResult)
	{
		case GamePlayer::Failed:
			stopGameplay();
			emit gameplayFailed(FailedToStart);
			break;

		case GamePlayer::ClosedByUser:
			stopGameplay();
			emit gameplayCancelled();
			break;

		default: //case GamePlayer::ClosedBySystem:
			emit gameplayCompleted();
			break;
	}
}

// +-----------------------------------------------------------
void gc::Application::onCaptureStarted()
{
	m_bWaitingForVideoFiles = true;
	m_oTimer.start(100);
}

// +-----------------------------------------------------------
void gc::Application::onCaptureEnded(gc::VideoCapturer::CaptureResult eResult)
{
	switch(eResult)
	{
		case VideoCapturer::FailedToStart:
			emit gameplayFailed(FailedToStart);
			break;

		case VideoCapturer::FailedToSave:
			m_bFailureSignalled = true;
			m_pGamePlayer->stopGameplay();
			emit gameplayFailed(FailedToConclude);
			break;

		default: // case VideoControl::Closed:
			m_pGamePlayer->stopGameplay();
			break;
	}
	
}

// +-----------------------------------------------------------
uint gc::Application::getGameplayReviewSamples() const
{
	return m_iGameplayReviewSamples;
}

// +-----------------------------------------------------------
uint gc::Application::getGameplayReviewInterval() const
{
	return m_iGameplayReviewInterval;
}

// +-----------------------------------------------------------
gc::GameplayData* gc::Application::getGameplayData()
{
	return &m_oGameplayData;
}

// +-----------------------------------------------------------
void gc::Application::setMainWindow(QWidget *pMainWindow)
{
	m_pMainWindow = pMainWindow;

#ifndef _DEBUG
	m_pMainWindow->setWindowFlags(m_pMainWindow->windowFlags() & ~Qt::WindowStaysOnBottomHint);
	m_pMainWindow->setWindowFlags(m_pMainWindow->windowFlags() | Qt::WindowStaysOnTopHint);
	m_pMainWindow->show();
#endif
}