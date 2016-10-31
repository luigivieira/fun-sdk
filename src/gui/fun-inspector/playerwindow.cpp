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

#include "playerwindow.h"
#include "csvfile.h"

// +-----------------------------------------------------------
fsdk::PlayerWindow::PlayerWindow(int iLandmarks, QWidget *pParent) :
	VideoWindow(pParent)
{
	for(int i = 0; i < iLandmarks; i++)
	{
		LandmarkWidget *pLandmark = new LandmarkWidget(i);
		m_lLandmarks.push_back(pLandmark);
		m_pVideoWidget->scene()->addItem(pLandmark);
		pLandmark->setVisible(false);
	}
}

// +-----------------------------------------------------------
void fsdk::PlayerWindow::mediaPositionChanged(qint64 iPosition)
{
	VideoWindow::mediaPositionChanged(iPosition);
	if(!m_mData.isEmpty())
	{
		int iFPS = 30;
		int iFrame = iFPS * (iPosition / 1000);
		QList<QPoint> lPositions = m_mData[iFrame];
		
		for(int i = 0; i <= m_lLandmarks.count(); i++)
		{
			if(i >= lPositions.count())
				break;

			m_lLandmarks[i]->setPos(lPositions[i]);
		}

		if(!landmarksVisible())
			setLandmarksVisible(true);
	}
}

// +-----------------------------------------------------------
void fsdk::PlayerWindow::landmarksFileChanged(const QString sFileName)
{
	if(sFileName.isEmpty())
	{
		m_mData.clear();
		setLandmarksVisible(false);
	}
	else
	{
		CSVFile oFile(sFileName);
		if(!oFile.read())
		{
			qWarning().noquote() << "Could not load the landmarks CSV file: " << sFileName;
			return;
		}

		QList<QStringList> lLines = oFile.lines();

		QMap<qint64, QList<QPoint>> mData;
		foreach(QStringList lLine, lLines)
		{
			int iFrame = lLine[0].toInt();
			QList<QPoint> lData;
			for(int i = 2; i < lLine.count() - 1; i += 2)
			{
				QPoint oMark(lLine[i].toInt(), lLine[i + 1].toInt());
				lData.push_back(oMark);
			}
			mData[iFrame] = lData;
		}

		m_mData.clear();
		m_mData = mData;
	}
}

// +-----------------------------------------------------------
bool fsdk::PlayerWindow::landmarksVisible() const
{
	if(m_lLandmarks.count() >= 1)
		return m_lLandmarks[0]->isVisible();
	else
		return false;
}

// +-----------------------------------------------------------
void fsdk::PlayerWindow::setLandmarksVisible(bool bVisible)
{
	foreach(LandmarkWidget *pMark, m_lLandmarks)
		pMark->setVisible(bVisible);
}