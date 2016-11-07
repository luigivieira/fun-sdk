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

#ifndef IMAGEMAN_H
#define IMAGEMAN_H

#include "libexport.h"
#include <QPixmap>
#include <opencv2\opencv.hpp>

namespace fsdk
{
	/**
	 * Useful image manipulation functions.
	 */
	class SHARED_LIB_EXPORT ImageMan
	{
	protected:
		/**
		 * Protected constructor, so the class can not be instantiated.
		 */
		ImageMan();

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

		/**
		 * Create a collated image from the mat images provided.
		 * @param lMats QList of OpenCV Mat instances with the images to collate.
		 * @param oMatSize OpenCV Size struct with the size of each collated image.
		 * The final collated image size will be based on the number of images, their
		 * row/column positioning and the width of the border.
		 * @param iRows Unsigned integer with the number of rows to use.
		 * @param iCols Unsigned integer with the number of columns to use.
		 * @param bMatResize Boolean indicating if the collated images should be resized
		 * to fit the given size in oMatSize. If this is set to false, the images will be
		 * croped when needed and centered inside the size provided in oMatSize. The
		 * default is true.
		 * @param oMatBkgColor OpenCV Scalar with the color to use for the background of
		 * each image area (visible only when bMatResize is set to false and the image is
		 * cropped). The default is Scalar(128).
		 * @param iMatBorderWidth Unsigned integer with the width of the border between each
		 * collated image, in range [0, 3]. The default is 0 (meaning no border).
		 * @param oMatBorderColor OpenCV Scalar with the color of the border (when iMatBorderWidth
		 * is different than 0). The default is Scalar(0).
		 * @param oImgBkgColor OpenCV Scalar with the background color for the whole collated image
		 * (visible when there are empty cells remaining). The default is Scalar(255).
		 * @param iImgType Integer with the image type to create the collated image. The default is
		 * CV_8UC1 (unsigned 8 bits and 1 channel).
		 * @return OpenCV Mat with the collated image created.
		 */
		static cv::Mat collateMats(const QList<cv::Mat> &lMats, cv::Size &oMatSize, uint iRows, uint iCols, bool bMatResize = true, cv::Scalar &oMatBkgColor = cv::Scalar(128), uint iMatBorderWidth = 0, cv::Scalar &oMatBorderColor = cv::Scalar(0), cv::Scalar &oImgBkgColor = cv::Scalar(255), int iImgType = CV_8UC1);
	};
}

#endif // IMAGEMAN_H