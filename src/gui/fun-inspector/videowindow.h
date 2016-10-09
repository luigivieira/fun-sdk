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

#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QDockWidget>
#include "framewidget.h"

namespace fsdk
{
    /**
     * Dockable window that displays the session videos (player and gameplay).
     */
    class VideoWindow : public QDockWidget
    {
        Q_OBJECT

    public:
        /**
         * Class constructor.
         * @param pParent QWidget with the window parent.
         */
        VideoWindow(QWidget *pParent = 0);

    private:
        
		/** Frame currently displayed at the window. */
		FrameWidget *m_pFrame;

    };
}

#endif // VIDEOWINDOW_H