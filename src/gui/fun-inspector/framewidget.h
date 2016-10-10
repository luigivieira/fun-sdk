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

#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>

namespace fsdk
{
    /**
     * Implements a widget that displays an image frame from a video.
	 * The frame is implemented using QGraphicsView and QGraphicsScene,
	 * so the frame image can be zoomed in and out and other graphic itens
	 * (facial landmarks, for instance) can easily be superimposed and
	 * even interacted with.
     */
    class FrameWidget : public QGraphicsView
    {
        Q_OBJECT

	public:
		/**
		 * Class constructor.
		 * @param pParent Instance of the parent widget.
		 */
		FrameWidget(QWidget *pParent = 0);

		/**
		 * Updates the pixmap displayed at the central area.
		 * @param oPixmap Reference for a QPixmap with the new pixmap to display.
		 */
		void setPixmap(const QPixmap &oPixmap);

		/**
		 * Gets the zoom level with which the image is being displayed.
		 * @return Double with the current zoom level.
		 */
		double zoomLevel() const;

		/**
		 * Sets the level of zoom to display the image with.
		 * @param dLevel Double with the zoom level in range [0.10, 10.0].
		 */
		void setZoomLevel(const double dLevel);

		/**
		 * Performs one step of zoom in (i.e. increases the zoom level by XX).
		 */
	    void zoomIn();

		/**
		 * Performs one step of zoom out (i.e. descreases the zoom level by XX).
		 */
		void zoomOut();

		/**
		 * Returns the size hint to be used by this widget.
		 */
		QSize sizeHint() const
		{
			return QSize(800, 600);
		};

	signals:

		/**
		 * Signal indicating changes in the zoom level of the image displayed.
		 * @param dLevel Double with the zoom level in range [0.10, 10.0].
		 */
		void zoomLevelChanged(const double dLevel);

	protected:

	#ifndef QT_NO_WHEELEVENT
		/**
		 * Captures and handles the mouse wheel events to automate user interactions.
		 * @param pEvent Pointer to a QWheelEvent with the mouse event.
		 */
		void wheelEvent(QWheelEvent *pEvent) Q_DECL_OVERRIDE;
	#endif

		/**
		 * Scales the widget view to emulate a zoom behaviour by the given level.
		 * @param dLevelStep Double with the value to be added/removed to the zoom level.
		 */
		void zoomBy(double dLevelStep);

	private:

		/** Scene used to render the widget contents. */
		QGraphicsScene *m_pScene;

		/** Pixmap item used to display the background image. */
		QGraphicsPixmapItem *m_pPixmapItem;

		/** Current zoom level. */
		double m_dZoomLevel;
	};
};


#endif // FRAMEWIDGET_H