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

#include "mediasynchronizer.h"

// +-----------------------------------------------------------
fsdk::MediaSynchronizer::MediaSynchronizer(QObject *pParent) : QObject(pParent)
{

}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::add(QMediaPlayer *pMediaPlayer)
{
	m_vMedias.push_back(pMediaPlayer);
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::remove(QMediaPlayer *pMediaPlayer)
{
	m_vMedias.removeOne(pMediaPlayer);
}

// +-----------------------------------------------------------
void fsdk::MediaSynchronizer::removeAll()
{
	m_vMedias.clear();
}