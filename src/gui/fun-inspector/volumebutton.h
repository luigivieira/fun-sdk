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

#ifndef VOLUME_BUTTON_H
#define VOLUME_BUTTON_H

#include <QToolButton>
#include <QLabel>
#include <QSlider>
#include <QMenu>
#include <QWidgetAction>

namespace fsdk
{
	/**
	 * Button used to control the audio volume of a video.
	 */
	class VolumeButton : public QToolButton
	{
		Q_OBJECT
	public:

        /**
         * Class constructor.
         * @param pParent Instance of a QWidget with the parent of this component.
         * The default is NULL.
         */
		VolumeButton(QWidget *pParent = NULL);

	public slots:

		/**
		 * Captures the slider value changed event.
		 * @iValue Integer with the current value of the slider (position of the handler).
		 */
		void onSliderValueChanged(int iValue);

	signals:

		/**
		 * Indicates that the volume has been changed by the user.
		 * @param iVolume Integer with the new volume set (in range [0, 100]).
		 */
		void volumeChanged(int iVolume);

	private:

		/** Popup menu used to display the slider upon click on the button. */
		QMenu *m_pMenu;

		/** Action used to set the volume. */
		QWidgetAction *m_pVolumeAction;

		/** Label used to display the volume value in percent. */
		QLabel *m_pLabel;

		/** Slider used to allow the user to change the volume. */
		QSlider *m_pSlider;
	};
}

#endif // VOLUME_BUTTON_H
