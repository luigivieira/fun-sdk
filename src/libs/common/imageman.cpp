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
#include <opencv2/imgproc/imgproc.hpp>

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
Mat fsdk::ImageMan::collateMats(const QList<Mat> &lMats, const Size &oMatSize, uint iRows, uint iCols, bool bMatResize, Scalar &oMatBkgColor, uint iMatBorderWidth, Scalar &oMatBorderColor, Scalar &oImgBkgColor, const QStringList &lXLabels, const QStringList &lYLabels, const QString &sXTitle, const QString &sYTitle, int iImgType)
{
	iMatBorderWidth = qMin((int) iMatBorderWidth, 3);

	int iFontFace = FONT_HERSHEY_SIMPLEX;
	double dFontScale = 0.5;
	int iThickness = 1;
	int iBaseline = 0;

	int iXLabelHeight = 0;
	foreach(QString sXLabel, lXLabels)
	{
		Size oFntSize = getTextSize(sXLabel.toStdString(), iFontFace, dFontScale, iThickness, &iBaseline);
		if(oFntSize.height > iXLabelHeight)
			iXLabelHeight = oFntSize.height;
	}
	iXLabelHeight *= 2;

	int iYLabelWidth = 0;
	foreach(QString sYLabel, lYLabels)
	{
		Size oFntSize = getTextSize(sYLabel.toStdString(), iFontFace, dFontScale, iThickness, &iBaseline);
		if(oFntSize.width > iYLabelWidth)
			iYLabelWidth = oFntSize.width;
	}
	iYLabelWidth *= 1.5;	

	int iXTitleHeight = 0;
	if(!sXTitle.isEmpty())
	{
		Size oFntSize = getTextSize(sXTitle.toStdString(), iFontFace, dFontScale, iThickness, &iBaseline);
		iXTitleHeight = oFntSize.height;
	}
	iXTitleHeight *= 2;

	int iYTitleHeight = 0;
	if(!sYTitle.isEmpty())
	{
		Size oFntSize = getTextSize(sYTitle.toStdString(), iFontFace, dFontScale, iThickness, &iBaseline);
		iYTitleHeight = oFntSize.height;
	}
	iYTitleHeight *= 2;

	// Create the collate image so it can hold all mat images + their borders and the labels
	Size oSize;
	oSize.width = iCols * oMatSize.width + (iCols + 1) * iMatBorderWidth + iYLabelWidth + iYTitleHeight;
	oSize.height = iRows * oMatSize.height + (iRows + 1) * iMatBorderWidth + iXLabelHeight + iXTitleHeight;

	Mat oCollateImage;
	oCollateImage.create(oSize, iImgType);
	oCollateImage = oImgBkgColor;
	
	// Draw the titles (if provided)
	if(!sYTitle.isEmpty())
	{
		Size oFntSize = getTextSize(sYTitle.toStdString(), iFontFace, dFontScale + 0.2, iThickness, &iBaseline);

		Mat oText;
		oText.create(Size(oSize.height, oSize.width), iImgType);
		oText = oImgBkgColor;
		
		// Draw the title text
		int x = iXLabelHeight - iXTitleHeight + (iRows * oMatSize.height + iRows * (iMatBorderWidth + 1)) / 2 - oFntSize.width / 2;
		int y = iXLabelHeight / 2 + oFntSize.height / 2;
		putText(oText, sYTitle.toStdString(), Point(x, y), iFontFace, dFontScale + 0.2, Scalar(0), iThickness);

		// Rotate it by 90 degrees
		Mat oRot = getRotationMatrix2D(Point2f(oSize.width / 2, oSize.height / 2), 90, 1.0);
		warpAffine(oText, oText, oRot, oSize, INTER_LINEAR, BORDER_CONSTANT, Scalar(255));

		// Translate to correct the difference in aspect ratio
		float fDx = oSize.width / 2 - oSize.height / 2;
		float fDy = oSize.height / 2 - oSize.width / 2;
		float aData[2][3] = { { 1.0f, 0.0f, -fDx },{ 0.0f, 1.0f, fDy } };
		Mat oTrans(2, 3, CV_32F, aData);
		warpAffine(oText, oText, oTrans, oSize, INTER_LINEAR, BORDER_CONSTANT, Scalar(255));	

		// Make the base image equal to the image with the text
		oCollateImage = oText;
	}
	if(!sXTitle.isEmpty())
	{
		// Draw the title text
		Size oFntSize = getTextSize(sXTitle.toStdString(), iFontFace, dFontScale + 0.2, iThickness, &iBaseline);
		int x = iYLabelWidth + iYTitleHeight + (iCols * oMatSize.width + iCols * (iMatBorderWidth + 1)) / 2 - oFntSize.width / 2;
		int y = iXLabelHeight / 2 + oFntSize.height / 2;
		putText(oCollateImage, sXTitle.toStdString(), Point(x, y), iFontFace, dFontScale + 0.2, Scalar(0), iThickness);
	}

	// Draw the labels (if provided)
	int iRow = 0;
	foreach(QString sYLabel, lYLabels)
	{
		Size oFntSize = getTextSize(sYLabel.toStdString(), iFontFace, dFontScale, iThickness, &iBaseline);

		int x = iYLabelWidth / 2 + iYTitleHeight - oFntSize.width / 2;
		int y = iXLabelHeight * 2 + iXTitleHeight + oFntSize.height + iRow * oMatSize.height + iRow * iMatBorderWidth;

		Point oPos(x, y);
		putText(oCollateImage, sYLabel.toStdString(), oPos, iFontFace, dFontScale, Scalar(0), iThickness);

		iRow++;
		if(iRow == iRows)
			break;
	}
	
	int iCol = 0;
	foreach(QString sXLabel, lXLabels)
	{
		Size oFntSize = getTextSize(sXLabel.toStdString(), iFontFace, dFontScale, iThickness, &iBaseline);

		int x = iYLabelWidth + iYTitleHeight + iCol * oMatSize.width + iCol * iMatBorderWidth;
		int y = iXLabelHeight + oFntSize.height * 1.5;

		Point oPos(x + oMatSize.width / 2 - oFntSize.width / 2, y);
		putText(oCollateImage, sXLabel.toStdString(), oPos, iFontFace, dFontScale, Scalar(0), iThickness);

		iCol++;
		if(iCol == iCols)
			break;
	}

	// Copy all images in the list into the collate image, according to
	// the requested rows and columns
	iRow = 0, iCol = 0;
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
		int x = iYLabelWidth + iYTitleHeight + iCol * oMatSize.width + iCol * iMatBorderWidth;
		int y = iXLabelHeight + iXTitleHeight + iRow * oMatSize.height + iRow * iMatBorderWidth;
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