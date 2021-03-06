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

#include "videowidget.h"

#include <QApplication>
#include <QPixmap>
#include <QGraphicsEffect>
#include <QScrollBar>
#include <QtMath>
#include <QGraphicsSceneMouseEvent>

// Scale values for zoom in and out steps
#define ZOOM_IN_STEP 1.25
#define ZOOM_OUT_STEP 0.80

// +-----------------------------------------------------------
fsdk::VideoWidget::VideoWidget(QWidget *pParent) : QGraphicsView(pParent)
{
	m_pScene = new QGraphicsScene(this);
	m_pScene->setItemIndexMethod(QGraphicsScene::NoIndex);
	setScene(m_pScene);

    scale(1.0, 1.0);
	m_dZoomLevel = 1.0;

	setBackgroundBrush(QApplication::palette().dark());

	// Add the video item
	m_pVideoItem = new QGraphicsVideoItem();
	m_pScene->addItem(m_pVideoItem);
}

// +-----------------------------------------------------------
QSize fsdk::VideoWidget::sizeHint() const
{
	return QSize(800, 600);
};

// +-----------------------------------------------------------
QGraphicsVideoItem *fsdk::VideoWidget::graphicsVideoItem()
{
	return m_pVideoItem;
}

// +-----------------------------------------------------------
double fsdk::VideoWidget::zoomLevel() const
{
	// Returns it with a precision of 5 decimals
	return qFloor(m_dZoomLevel * 100000.0) / 100000.0;
}

// +-----------------------------------------------------------
void fsdk::VideoWidget::setZoomLevel(const double dLevel)
{
	if(dLevel == m_dZoomLevel)
		return;

	if(dLevel >= 0.10 && dLevel <= 10.0)
	{
		// First, go back to the base scale (1.0 or 100%)
		double dAdjust = 1.0 / m_dZoomLevel;
		scale(dAdjust, dAdjust);

		// Then, apply the requested zoom level.
		m_dZoomLevel = dLevel;
		if(m_dZoomLevel != 1.0)
			scale(dLevel, dLevel);

		// Emit the signal that the zoom level has changed
		emit zoomLevelChanged(zoomLevel());
	}
}

// +-----------------------------------------------------------
void fsdk::VideoWidget::zoomBy(double dLevelBy)
{
	double dLevel = m_dZoomLevel * dLevelBy;
	if(dLevel >= 0.10 && dLevel <= 10.0)
	{
		m_dZoomLevel = dLevel;
	    scale(dLevelBy, dLevelBy);

		// Emit the signal that the zoom level has changed
		emit zoomLevelChanged(zoomLevel());
	}
}

#ifndef QT_NO_WHEELEVENT
// +-----------------------------------------------------------
void fsdk::VideoWidget::wheelEvent(QWheelEvent *pEvent)
{
	bool bCtrl = QApplication::keyboardModifiers() & Qt::ControlModifier;
	bool bAlt = QApplication::keyboardModifiers() & Qt::AltModifier;
	bool bShift = QApplication::keyboardModifiers() & Qt::ShiftModifier;
	int iDelta = pEvent->angleDelta().x() + pEvent->angleDelta().y();
	double dBase = iDelta < 0 ? ZOOM_OUT_STEP : ZOOM_IN_STEP;
	int iSteps = abs(iDelta / 120);

	if(!(bCtrl || bAlt || bShift)) // No special key pressed => scroll vertically
		verticalScrollBar()->setValue(verticalScrollBar()->value() - iDelta);
	else if(bShift && !(bCtrl || bAlt)) // Only shift key pressed => scroll horizontally
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - iDelta);
	else if(bCtrl && !(bAlt || bShift)) // Only ctrl key pressed => zoom in and out
		zoomBy(qPow(dBase, iSteps));
}
#endif

// +-----------------------------------------------------------
void fsdk::VideoWidget::zoomIn()
{
	zoomBy(ZOOM_IN_STEP);
}

// +-----------------------------------------------------------
void fsdk::VideoWidget::zoomOut()
{
	zoomBy(ZOOM_OUT_STEP);
}

// +-----------------------------------------------------------
void fsdk::VideoWidget::mediaStatusChanged(QMediaPlayer::MediaStatus eStatus)
{
	Q_UNUSED(eStatus);
	m_pVideoItem->setSize(m_pVideoItem->nativeSize());
}