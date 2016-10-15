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

#ifndef CUSTOMSLIDER_H
#define CUSTOMSLIDER_H

#include <QSlider>
#include <QMouseEvent>
#include <QStyleOptionSlider>

namespace fsdk
{
	/**
	 * Custom horizontal QSlider with "jump click" (i.e. directly
	 * moves the slider handle to where the user clicked on the
	 * slider trail).
	 */
	class CustomSlider : public QSlider
	{
	public:
		/**
		 * Class constructor.
		 * @param pParent Instance of a QWidget with the parent.
		 */
		CustomSlider(QWidget *pParent);

	protected:
		/**
		 * Captures the mouse press event to implement the , "jump click".
		 * @param pEvent Instance of the QMouseEvent with the event data.
		 */
		void mousePressEvent(QMouseEvent *pEvent);
	};
}

#endif // CUSTOMSLIDER_H