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

#ifndef LANDMARKWIDGET_H
#define LANDMARKWIDGET_H

#include <QGraphicsItem>

namespace fsdk
{
    /**
     * Implements a widget that represents a facial landmark on the VideoWindow.
     */
    class LandmarkWidget : public QGraphicsItem
    {
	public:
		/**
		 * Class constructor.
		 * @param iNumber Integer to number the landmark with. The default is 0.
		 */
		LandmarkWidget(uint iNumber = 0);

		/**
		 * Gets the bounding rectangle of the landmark according to its position and radius.
		 * @return A QRectF with the coordinates and size of the bounding rect of the landmark.
		 */
		QRectF boundingRect() const;

		/** 
		 * Gets the radius of the landmark.
		 * @return Integer with the radius of the landmark.
		 */
		int radius() const;

		/**
		 * Sets the radius of the landmark.
		 * @param iRadius Integer with the radius of the landmark.
		 */
		void setRadius(int iRadius);

	signals:

		

	public slots:

		

	protected:

		/**
		 * Implements the paint method to draw the landmark.
		 * @param pPainter Instance of a QPainter to allow drawing.
		 * @param pOption Instance of a QStyleOptionGraphicsItem with information on the style and state.
		 * @param pWidget Instance of a QWidget with the widget that the landmark is being painted on. Optional (might be 0).
		 */
		void paint(QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget);

		/**
		 * Captures changes in the landmark.
		 * @param eChange GraphicsItemChange enumeration value indicating what the change is.
		 * @param oValue QVariant const reference with the value that changed.
		 */
		QVariant itemChange(GraphicsItemChange eChange, const QVariant &oValue);

	private:

		/** Number of the landmark. */
		uint m_iNumber;

		/** Radius in pixels used to set the size of the landmark. */
		int m_iRadius;
	};
};

#endif // LANDMARKWIDGET_H