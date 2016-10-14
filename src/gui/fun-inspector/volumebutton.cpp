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

#include "volumebutton.h"
#include <QBoxLayout>
#include <QDebug>
#include <QMouseEvent>
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionSlider>

#define SLIDER_STYLE_SHEET "\
	fsdk--VolumeButton QSlider::groove:horizontal {\
		border: 1px solid #999999;\
		height: 9px;\
		background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #b1b1b1, stop: 1 #c4c4c4);\
		margin: 2px 2px;\
		border-radius: 4px;\
	}\
	\
	fsdk--VolumeButton QSlider::handle:horizontal {\
		background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #d96459, stop:1 #8c4646);\
		border: 1px solid #5c5c5c;\
		width: 11px;\
		margin: -2px -2px;\
		border-radius: 6px;\
	}"

namespace fsdk
{
	/**
	 * Custom horizontal QSlider used for setting the volume.
	 */
	class VolumeSlider : public QSlider
	{
	public:
		/**
		 * Class constructor.
		 * @param pParent Instance of a VolumeButton with the parent.
		 */
		VolumeSlider(VolumeButton *pParent) : QSlider(Qt::Horizontal, pParent)
		{
			setRange(0, 100);
			setPageStep(10);
			setStyleSheet(SLIDER_STYLE_SHEET);
			connect(this, &QAbstractSlider::valueChanged, pParent, &VolumeButton::sliderValueChanged);
		}

	protected:
		/**
		 * Captures the mouse press event, so the slider can be "jumped" to
		 * the any position clicked by the user on the slider bar.
		 * @param pEvent Instance of the QMouseEvent with the event data.
		 */
		void mousePressEvent(QMouseEvent *pEvent)
		{
			QStyleOptionSlider oOpt;
			initStyleOption(&oOpt);
			QRect oHandleRect = style()->subControlRect(QStyle::CC_Slider, &oOpt, QStyle::SC_SliderHandle, this);

			if(pEvent->button() == Qt::LeftButton && oHandleRect.contains(pEvent->pos()) == false)
			{
				// Implements the "jump click": directly moves the slider handle
				// to where the user clicked on the slider trail
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
	};
}

// +-----------------------------------------------------------
fsdk::VolumeButton::VolumeButton(QWidget *pParent) : QToolButton(pParent)
{
	setPopupMode(QToolButton::InstantPopup);

	QWidget *pPopup = new QWidget(this);

	m_pSlider = new VolumeSlider(this);

	m_pLabel = new QLabel(this);
	m_pLabel->setAlignment(Qt::AlignCenter);
	m_pLabel->setText("100%");
	m_pLabel->setMinimumWidth(m_pLabel->sizeHint().width());

	QBoxLayout *pPopupLayout = new QHBoxLayout(pPopup);
	pPopupLayout->setMargin(2);
	pPopupLayout->addWidget(m_pSlider);
	pPopupLayout->addWidget(m_pLabel);

	m_pVolumeAction = new QWidgetAction(this);
	m_pVolumeAction->setDefaultWidget(pPopup);

	m_pMenu = new QMenu(this);
	m_pMenu->addAction(m_pVolumeAction);
	setMenu(m_pMenu);

	setVolume(100);
}

// +-----------------------------------------------------------
void fsdk::VolumeButton::sliderValueChanged(int iValue)
{
	setVolume(iValue);
	emit volumeChanged(iValue);
}

// +-----------------------------------------------------------
int fsdk::VolumeButton::volume() const
{
	return m_pSlider->value();
}

// +-----------------------------------------------------------
void fsdk::VolumeButton::setVolume(int iValue)
{
	m_pLabel->setText(QString("%1%").arg(iValue));
	if(iValue == 0)
		setIcon(QIcon(":/icons/audio-muted.png"));
	else if(iValue <= 32)
		setIcon(QIcon(":/icons/audio-min.png"));
	else if(iValue <= 65)
		setIcon(QIcon(":/icons/audio-med.png"));
	else
		setIcon(QIcon(":/icons/audio-max.png"));
	m_pSlider->setValue(iValue);

	refreshUI();
}

// +-----------------------------------------------------------
void fsdk::VolumeButton::refreshUI()
{
	setToolTip(tr("Volume: %1%").arg(volume()));
	setStatusTip(tr("Sets the audio volume in this video window"));
}