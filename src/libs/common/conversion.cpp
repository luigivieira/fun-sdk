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

#include "conversion.h"
#include <QDebug>

// +-----------------------------------------------------------
fsdk::Conversion::Conversion()
{
}

// +-----------------------------------------------------------
QImage fsdk::Conversion::Mat2Image(const cv::Mat &oMat)
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
			qWarning().noquote() << "fsdk::Conversion::Mat2Image() - cv::Mat image type not currently handled: " << oMat.type();
			return QImage();
	}
}

// +-----------------------------------------------------------
QPixmap fsdk::Conversion::Mat2Pixmap(const cv::Mat &oMat)
{
	return QPixmap::fromImage(Mat2Image(oMat));
}

// +-----------------------------------------------------------
cv::Mat fsdk::Conversion::Image2Mat(const QImage &oImage)
{
	switch(oImage.format())
	{
		// 8-bit, 4 channel
		case QImage::Format_ARGB32:
		case QImage::Format_ARGB32_Premultiplied:
			return cv::Mat(oImage.height(), oImage.width(), CV_8UC4, const_cast<uchar*>(oImage.bits()), static_cast<size_t>(oImage.bytesPerLine())).clone();

		// 8-bit, 3 channel
		case QImage::Format_RGB32:
		case QImage::Format_RGB888:
		{
			QImage oSwapped;
			if(oImage.format() == QImage::Format_RGB32)
				oSwapped = oImage.convertToFormat(QImage::Format_RGB888);
			oSwapped = oImage.rgbSwapped();
			return cv::Mat(oSwapped.height(), oSwapped.width(), CV_8UC3, const_cast<uchar*>(oSwapped.bits()), static_cast<size_t>(oSwapped.bytesPerLine())).clone();
		}

		// 8-bit, 1 channel
		case QImage::Format_Indexed8:
			return cv::Mat(oImage.height(), oImage.width(), CV_8UC1, const_cast<uchar*>(oImage.bits()), static_cast<size_t>(oImage.bytesPerLine())).clone();

		// All other types are yet unhandled
		default:
			qWarning().noquote() << "fsdk::Conversion::Image2Mat() - QImage image type not currently handled: " << oImage.format();
			return cv::Mat();
	}
}

// +-----------------------------------------------------------
cv::Mat fsdk::Conversion::Pixmap2Mat(const QPixmap &oPixmap)
{
	return Image2Mat(oPixmap.toImage());
}