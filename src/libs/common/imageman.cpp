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

#include "imageman.h"
#include <QDebug>

using namespace cv;

// +-----------------------------------------------------------
fsdk::ImageMan::ImageMan()
{
}

// +-----------------------------------------------------------
QImage fsdk::ImageMan::Mat2Image(const Mat &oMat)
{
	switch(oMat.type())
	{
		// 8-bit, 4 channel
		case CV_8UC4:
			return QImage(oMat.data, oMat.cols, oMat.rows, static_cast<int>(oMat.step), QImage::Format_ARGB32);

		// 8-bit, 3 channel
		case CV_8UC3:
			return QImage(oMat.data, oMat.cols, oMat.rows, static_cast<int>(oMat.step), QImage::Format_RGB888);

		// 8-bit, 1 channel
		case CV_8UC1:
		{
			static QVector<QRgb> vColorTable(256); // Memory-cached colour table
			if(vColorTable.isEmpty())
				for(int i = 0; i < 256; ++i)
					vColorTable[i] = qRgb(i, i, i);

			QImage oRet = QImage(oMat.data, oMat.cols, oMat.rows, static_cast<int>(oMat.step), QImage::Format_Indexed8);
			oRet.setColorTable(vColorTable);
			return oRet;
		}

		// All other types are yet unhandled
		default:
			qWarning().noquote() << "fsdk::ImageMan::Mat2Image() - cv::Mat image type not currently handled: " << oMat.type();
			return QImage();
	}
}

// +-----------------------------------------------------------
QPixmap fsdk::ImageMan::Mat2Pixmap(const Mat &oMat)
{
	return QPixmap::fromImage(Mat2Image(oMat));
}

// +-----------------------------------------------------------
Mat fsdk::ImageMan::Image2Mat(const QImage &oImage)
{
	switch(oImage.format())
	{
		// 8-bit, 4 channel
		case QImage::Format_ARGB32:
		case QImage::Format_ARGB32_Premultiplied:
			return Mat(oImage.height(), oImage.width(), CV_8UC4, const_cast<uchar*>(oImage.bits()), static_cast<size_t>(oImage.bytesPerLine())).clone();

		// 8-bit, 3 channel
		case QImage::Format_RGB32:
		case QImage::Format_RGB888:
		{
			QImage oSwapped;
			if(oImage.format() == QImage::Format_RGB32)
				oSwapped = oImage.convertToFormat(QImage::Format_RGB888);
			oSwapped = oImage.rgbSwapped();
			return Mat(oSwapped.height(), oSwapped.width(), CV_8UC3, const_cast<uchar*>(oSwapped.bits()), static_cast<size_t>(oSwapped.bytesPerLine())).clone();
		}

		// 8-bit, 1 channel
		case QImage::Format_Indexed8:
			return Mat(oImage.height(), oImage.width(), CV_8UC1, const_cast<uchar*>(oImage.bits()), static_cast<size_t>(oImage.bytesPerLine())).clone();

		// All other types are yet unhandled
		default:
			qWarning().noquote() << "fsdk::ImageMan::Image2Mat() - QImage image type not currently handled: " << oImage.format();
			return Mat();
	}
}

// +-----------------------------------------------------------
Mat fsdk::ImageMan::Pixmap2Mat(const QPixmap &oPixmap)
{
	return Image2Mat(oPixmap.toImage());
}

// +-----------------------------------------------------------
Mat fsdk::ImageMan::collateMats(const QList<Mat> &lMats, const Size &oMatSize, uint iRows, uint iCols, bool bMatResize, Scalar &oMatBkgColor, uint iMatBorderWidth, Scalar &oMatBorderColor, Scalar &oImgBkgColor, int iImgType)
{
	iMatBorderWidth = qMin((int) iMatBorderWidth, 3);

	// Create the collate image so it can hold all mat images
	Size oSize;
	oSize.width = iCols * oMatSize.width + (iCols + 1) * iMatBorderWidth;
	oSize.height = iRows * oMatSize.height + (iRows + 1) * iMatBorderWidth;

	Mat oCollateImage;
	oCollateImage.create(oSize, iImgType);
	oCollateImage = oImgBkgColor;

	// Copy all images in the list into the collate image, according to
	// the requested rows and columns
	int iRow = 0, iCol = 0;
	foreach(Mat oImage, lMats)
	{
		// Prepare the current image before copying it into the collated image
		oImage.convertTo(oImage, iImgType);
		if(oMatSize != oImage.size())
		{
			// Simply resize the image if requested
			if(bMatResize)
				resize(oImage, oImage, oMatSize);
			else
			{
				// Crop the image horizontally and vertically if needed
				Rect oClip(0, 0, oImage.size().width, oImage.size().height);
				if(oImage.size().width > oMatSize.width)
				{
					int iDiff = oImage.size().width - oMatSize.width;
					oClip.x += iDiff / 2;
					oClip.width -= iDiff;
				}
				if(oImage.size().height > oMatSize.height)
				{
					int iDiff = oImage.size().height - oMatSize.height;
					oClip.y += iDiff / 2;
					oClip.height -= iDiff;
				}
				if(oClip.x != 0 || oClip.y != 0)
					oImage = oImage(oClip).clone();

				// Center the image over another image of the requested size and
				// with the requested background color
				Mat oAux;
				oAux.create(oMatSize, iImgType);
				oAux = oMatBkgColor;
				int x = oMatSize.width / 2 - oImage.size().width / 2;
				int y = oMatSize.height / 2 - oImage.size().height / 2;
				oClip = Rect(x, y, oImage.size().width, oImage.size().height);
				oImage.copyTo(oAux(oClip));
				oImage = oAux;
			}
		}

		// Copy current image over the collated image
		int x = iCol * oMatSize.width + iCol * iMatBorderWidth;
		int y = iRow * oMatSize.height + iRow * iMatBorderWidth;
		if(iMatBorderWidth > 0)
		{
			Rect oBorder(x, y, oMatSize.width + 2 * iMatBorderWidth, oMatSize.height + 2 * iMatBorderWidth);
			rectangle(oCollateImage, oBorder, oMatBorderColor, iMatBorderWidth);
		}

		Rect oClip(x + iMatBorderWidth, y + iMatBorderWidth, oMatSize.width, oMatSize.height);
		oImage.copyTo(oCollateImage(oClip));

		iCol++;
		if(iCol == iCols)
		{
			iCol = 0;
			iRow++;
			if(iRow == iRows)
				break;
		}
	}

	return oCollateImage;
}