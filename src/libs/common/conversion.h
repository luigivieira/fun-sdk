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

#ifndef CONVERSION_H
#define CONVERSION_H

#include "libexport.h"
#include <QPixmap>
#include <opencv2\opencv.hpp>

namespace fsdk
{
	/**
	 * Useful conversion functions.
	 */
	class SHARED_LIB_EXPORT Conversion
	{
	protected:
		/**
		 * Protected constructor, so the class can not be instantiated.
		 */
		Conversion();

	public:
		/**
		 * Converts an image from OpenCV's Mat format to Qt's QPixmap format.
		 * Obs.: This conversion is based on the code from Andy Maloney (Nov. 2013):
		 * https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
		 *
		 * @param oMat The OpenCV's Mat with the image data to be converted.
		 * @return A Qt's QPixmap with the image data converted.
		 */
		static QPixmap Mat2Pixmap(const cv::Mat &oMat);

		/**
		 * Converts an image from OpenCV's Mat format to Qt's QImage format.
		 * Obs.: This conversion is based on the code from Andy Maloney (Nov. 2013):
		 * https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
		 *
		 * @param oMat The OpenCV's Mat with the image data to be converted.
		 * @return A Qt's QImage with the image data converted.
		 */
		static QImage Mat2Image(const cv::Mat &oMat);

		/**
		 * Converts an image from Qt's QPixmap format to OpenCV's Mat format.
		 * Obs.: This conversion is based on the code from Andy Maloney (Nov. 2013):
		 * https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
		 *
		 * @param oPixmap The Qt's QPixmap with the image to be converted.
		 * @return An OpenCV's Mat with the image data data converted.
		 */
		static cv::Mat Pixmap2Mat(const QPixmap &oPixmap);

		/**
		 * Converts an image from Qt's QImage format to OpenCV's Mat format.
		 * Obs.: This conversion is based on the code from Andy Maloney (Nov. 2013):
		 * https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
		 *
		 * @param oImage The Qt's QImage with the image to be converted.
		 * @return An OpenCV's Mat with the image data data converted.
		 */
		static cv::Mat Image2Mat(const QImage &oImage);
	};
}

#endif // CSVFILE_H