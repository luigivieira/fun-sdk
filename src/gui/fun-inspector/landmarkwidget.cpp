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

#include "landmarkwidget.h"
#include "application.h"
#include <QPainter>
#include <QGraphicsScene>

// +-----------------------------------------------------------
fsdk::LandmarkWidget::LandmarkWidget(uint iNumber) : QGraphicsItem()
{
	setSelected(false);
	setAcceptHoverEvents(true);
	m_iNumber = iNumber;
	m_iRadius = 4;
}

// +-----------------------------------------------------------
QRectF fsdk::LandmarkWidget::boundingRect() const
{
	return QRectF(-m_iRadius, -m_iRadius, 2 * m_iRadius, 2 * m_iRadius);
}

// +-----------------------------------------------------------
void fsdk::LandmarkWidget::paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget)
{
	Q_UNUSED(pOption);
	Q_UNUSED(pWidget);

	QBrush oBrush;
	oBrush.setStyle(Qt::SolidPattern);
	pPainter->setPen(QPen(Qt::yellow, 0));
	oBrush.setColor(QColor(Qt::yellow));

	pPainter->setBrush(oBrush);
	pPainter->drawEllipse(boundingRect());
}

// +-----------------------------------------------------------
QVariant fsdk::LandmarkWidget::itemChange(GraphicsItemChange eChange, const QVariant &oValue)
{
	if(eChange == ItemPositionChange)
		setToolTip(Application::translate("LandmarkWidget", "#%1 at (%2, %3)").arg(QString::number(m_iNumber), QString::number(pos().x()), QString::number(pos().y())));
	return QGraphicsItem::itemChange(eChange, oValue);
}

// +-----------------------------------------------------------
int fsdk::LandmarkWidget::radius() const
{
	return m_iRadius;
}

// +-----------------------------------------------------------
void fsdk::LandmarkWidget::setRadius(int iRadius)
{
	m_iRadius = qMax(2, iRadius);
	scene()->invalidate();
}