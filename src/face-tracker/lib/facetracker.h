/*
 * Copyright (C) 2016 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
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

#ifndef FACETRACKER_H
#define FACETRACKER_H

#include "libexport.h"
#include <opencv2\core\core.hpp>

using namespace cv;

namespace fsdk
{
	/**
	 * Defines the basic face tracking interface.
	 * Specific implementations must inherit this class
	 * and implement its pure virtual methods.
	 */
	class LIBEXPORT FaceTracker
	{
	public:
		/**
		 * Tracks a face in the given frame. If the frame is the first one
		 * (after the class has been instantiated or a call to reset() has 
		 * been performed), the face is firstly detected. If the frame is
		 * a new frame after a previous successful detection/tracking,
		 * the face model is ajusted to the face's new position.
		 * @param oFrame Constant reference to an OpenCV's Mat with the
		 * data of the image where the face is to be found.
		 */
		virtual void track(const Mat &oFrame) = 0;

		/**
		 * Queries the quality of the current tracking.
		 * @return Float between 0 and 1 indicating the quality of the
		 * current tracking. A value of 1 indicates a perfect tracking
		 * (that is, the face is found as best as possible and the features
		 * are correctly posisioned), and a value of 0 indicates total failure
		 * in the tracking (a face could not be tracked from a previous frame,
		 * case in which a call to reset() might be helpful).
		 */
		virtual float getQuality() = 0;

		/**
		 * Resets the tracking by attempting to redetect the face in the next
		 * frame.
		 */
		virtual void reset() = 0;
	};
}

#endif // FACETRACKER_H