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

#include "customslider.h"
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionSlider>

// +-----------------------------------------------------------
fsdk::CustomSlider::CustomSlider(QWidget *pParent) : QSlider(Qt::Horizontal, pParent)
{
	setRange(0, 100);
	setPageStep(10);
}

// +-----------------------------------------------------------
void fsdk::CustomSlider::mousePressEvent(QMouseEvent *pEvent)
{
	QStyleOptionSlider oOpt;
	initStyleOption(&oOpt);
	QRect oHandleRect = style()->subControlRect(QStyle::CC_Slider, &oOpt, QStyle::SC_SliderHandle, this);

	if(pEvent->button() == Qt::LeftButton && oHandleRect.contains(pEvent->pos()) == false)
	{
		double dHalfHandleWidth = (0.5 * oHandleRect.width()) + 0.5;
		int iAdaptedPosX = pEvent->x();
		if(iAdaptedPosX < dHalfHandleWidth)
			iAdaptedPosX = dHalfHandleWidth;
		if(iAdaptedPosX > width() - dHalfHandleWidth)
			iAdaptedPosX = width() - dHalfHandleWidth;

		double dNewWidth = (width() - dHalfHandleWidth) - dHalfHandleWidth;
		double normalizedPosition = (iAdaptedPosX - dHalfHandleWidth) / dNewWidth;

		int iNewVal = minimum() + ((maximum() - minimum()) * normalizedPosition);
		if(invertedAppearance() == true)
			setValue(maximum() - iNewVal);
		else
			setValue(iNewVal);

		pEvent->accept();
		emit actionTriggered(QSlider::SliderMove);
	}
	QSlider::mousePressEvent(pEvent);
}