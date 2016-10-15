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
	QSlider::groove:horizontal {\
		border: 1px solid #999999;\
		height: 9px;\
		background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #b1b1b1, stop: 1 #c4c4c4);\
		margin: 2px 2px;\
		border-radius: 4px;\
	}\
	\
	QSlider::handle:horizontal {\
		background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #d96459, stop:1 #8c4646);\
		border: 1px solid #5c5c5c;\
		width: 11px;\
		margin: -2px -2px;\
		border-radius: 6px;\
	}"

// +-----------------------------------------------------------
fsdk::VolumeButton::VolumeButton(QWidget *pParent) : QToolButton(pParent)
{
	setPopupMode(QToolButton::InstantPopup);

	QWidget *pPopup = new QWidget(this);

	m_pSlider = new CustomSlider(this);
	m_pSlider->setStyleSheet(SLIDER_STYLE_SHEET);
	connect(m_pSlider, &QAbstractSlider::valueChanged, this, &VolumeButton::sliderValueChanged);

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