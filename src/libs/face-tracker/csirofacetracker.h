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

#ifndef CSIROFACETRACKER_H
#define CSIROFACETRACKER_H

#include "facetracker.h"

// Predefinition of the CSIRO face tracker classes
namespace FACETRACKER
{
	class FaceTracker;
	class FaceTrackerParams;
}

namespace fsdk
{
	/**
	 * Defines the basic face tracking interface.
	 * Specific implementations must inherit this class
	 * and implement its pure virtual methods.
	 */
	class SHARED_LIB_EXPORT CSIROFaceTracker: FaceTracker
	{
	public:
		/**
		 * Default constructor.
		 */
		CSIROFaceTracker();

		/**
		 * Class destructor.
		 */
		virtual ~CSIROFaceTracker();

		/**
		 * Tracks a face in the given frame. If the frame is the first one
		 * (after the class has been instantiated or a call to reset() has
		 * been performed), the face is firstly detected. If the frame is
		 * a new frame after a previous successful detection/tracking,
		 * the face model is ajusted to the face's new position.
		 * @param oFrame Reference to an OpenCV's Mat with the
		 * data of the image where the face is to be found.
		 */
		void track(Mat &oFrame);

		/**
		 * Queries the quality of the current tracking.
		 * @return Float between 0 and 1 indicating the quality of the
		 * current tracking. A value of 1 indicates a perfect tracking
		 * (that is, the face is found as best as possible and the features
		 * are correctly posisioned), and a value of 0 indicates total failure
		 * in the tracking (a face could not be tracked from a previous frame,
		 * case in which a call to reset() might be helpful).
		 */
		float getQuality() const;

		/**
		 * Queries the positions of the currently tracked facial landmarks.
		 * @return Q QList of QPoint values for all facial landmarks tracked,
		 * or an empty QList() in case the quality of the tracker is 0 or too
		 * small for the tracking to worker properly.
		 */
		QList<QPoint> getLandmarks() const;

		/**
		 * Queries the total number of facial landmarks supported by the
		 * implemented tracker. This method should be overwritten by the
		 * children classes. The CSIRO Face Tracking SDK uses 66 landmarks.
		 * @return Unsigned integer with the number of landmarks supported.
		 */
		static uint landmarksCount() { return 66; };

		/**
		 * Resets the tracking by attempting to find the face again in the next
		 * frame.
		 */
		void reset();

	private:

		/** Instance of the CSIRO Face Tracker. */
		FACETRACKER::FaceTracker *m_pTracker;

		/** Instance of the CSIRO Face Tracker parameters. */
		FACETRACKER::FaceTrackerParams *m_pTrackerParams;

		/** Current quality of the tracking. */
		float m_fQuality;

		/** Cached list of the landmarks obtained from the previous tracking. */
		QList<QPoint> m_lLandmarks;
	};
}

#endif // CSIROFACETRACKER_H