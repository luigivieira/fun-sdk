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

#ifndef GABORKERNEL_H
#define GABORKERNEL_H

#include "libexport.h"
#include <opencv2\opencv.hpp>

namespace fsdk
{
	/**
	 * Implements a complex Gabor Kernel (i.e. with both real and imaginary components),
	 * useful to filter images by convolution. This implementation is needed because
	 * the existing function available in OpenCV only returns the real part of the kernel.
	 * Both real and imaginary components are needed to calculate the energy (i.e. the
	 * magnitude of both components as sqrt(R^2 + I^2)) for the filter responses to be
	 * useful in representing facial expressions.
	 */
	class SHARED_LIB_EXPORT GaborKernel
	{
	public:

		/**
		 * Default class constructor. Creates an empty kernel.
		 */
		GaborKernel();

		/**
		 * Class constructor. Creates a kernel from the given parameters.		 
		 * @param iWindowSize Integer with the size of the kernel window (in pixels).
		 * This value is used for both the width and height of the kernel, since this
		 * implementation only supports squared (aspect ratio = 1) kernels.
		 * @param dTheta Double with the theta (θ) value used for the direction of the
		 * Sinusoidal carrier of the kernel (in radians).
		 * @param dLambda Double with the lambda (λ) value used for the wavelength of the
		 * Sinusoidal carrier of the kernel (in pixels).
		 * @param dSigma Double with the sigma (σ) value used for the Gaussian envelope
		 * of the kernel (in radians).
		 * @param dPsi Double with the psi (ψ) value used for the wave phase offset of the
		 * Sinusoidal carrier of the kernel (in radians).
		 */
		GaborKernel(const int iWindowSize, const double dTheta, const double dLambda, const double dSigma, const double dPsi);

		/**
		 * Class constructor. Creates a kernel from the given parameters.
		 * In this implementation, sigma (σ) and the kernel size are automatically calculated.
		 * Sigma is calculated from lambda (λ) and a fixed bandwidth of one octave, as it is
		 * a standard in the literature (supposedly this mimics well the visual system of
		 * animals - for further details, refer to this link: http://www.cs.rug.nl/~imaging/simplecell.html).
		 * The kernel size is calculated from sigma in order to limit the cutoff to a maximum
		 * of 0.5%.
		 * @param dTheta Double with the theta (θ) value used for the direction of the
		 * Sinusoidal carrier of the kernel (in radians).
		 * @param dLambda Double with the lambda (λ) value used for the wavelength of the
		 * Sinusoidal carrier of the kernel (in pixels).
		 * @param dPsi Double with the psi (ψ) value used for the wave phase offset of the
		 * Sinusoidal carrier of the kernel (in radians). The default is PI/2 (another standard in the
		 * literature).
		 */
		GaborKernel(const double dTheta, const double dLambda, const double dPsi = CV_PI / 2);

		/**
		 * Copy constructor.
		 * @param oOther Constant reference to the other GaborKernel from where
		 * to copy data from.
		 */
		GaborKernel(const GaborKernel &oOther);

		/**
		 * Assignment operator.
		 * @param oOther Constant reference to the other GaborKernel from where
		 * to copy data from.
		 */
		GaborKernel& operator=(const GaborKernel &oOther);

		/**
		 * Gets the value of the theta (θ) parameter, used in the kernel to 
		 * define the orientation of the sinusoidal carrier.
		 * @return Double with the orientation of the sinusoidal carrier
		 * in radians.
		 */
		double theta() const;

		/**
		 * Sets the value of the theta (θ) parameter, used in the kernel to
		 * define the orientation of the sinusoidal carrier.
		 * @param dValue Double with the orientation of the sinusoidal carrier
		 * in radians and in range [0, PI].
		 */
		void setTheta(const double dValue);

		/**
		 * Gets the value of the lambda (λ) parameter, used in the kernel to 
		 * define the wavelength of the sinusoidal carrier.
		 * @return Double with the wavelength of the sinusoidal carrier
		 * in pixels.
		 */
		double lambda() const;

		/**
		 * Sets the value of the lambda (λ) parameter, used in the kernel to
		 * define the wavelength of the sinusoidal carrier in pixels.
		 * @param dValue Double with the wavelength of the sinusoidal carrier
		 * in pixels. The minimum value accepted is 3. P.S.: Even though there
		 * is no fixed maximum, the value of lambda *should not* be greater than
		 * W/2 (where W is the size of the image which the kernel will be convolved
		 * with), otherwise the filtering may produce weird results.
		 */
		void setLambda(const double dValue);

		/**
		 * Gets the value of the psi (ψ) parameter, used in the kernel to
		 * define the phase of the sinusoidal carrier.
		 * @return Double with the phase of the sinusoidal carrier
		 * in radians.
		 */
		double psi() const;

		/**
		 * Sets the value of the psi (ψ) parameter, used in the kernel to
		 * define the phase of the sinusoidal carrier.
		 * @param dValue Double with the phase of the sinusoidal carrier
		 * in radians and in range [0, PI].
		 */
		void setPsi(const double dValue);

		/**
		 * Gets the value of the sigma (σ) parameter, used to define the
		 * standard deviation (i.e. the size) of the Gaussian envelope.
		 * @return Double with the standard deviation of the Gaussian envelope
		 * in radians.
		 */
		double sigma() const;

		/**
		 * Sets the value of the sigma (σ) parameter, used to define
		 * the standard variation (i.e. the cutoff) of the Gaussian envelope,
		 * in radians.
		 * @param dValue Double with the standard deviation of the Gaussian
		 * envelope. The minimum value accepted is 0.
		 */
		void setSigma(const double dValue);

		/**
		 * Gets the window size (both width and height) of the kernel.
		 * @return Integer with the size of the kernel size in pixels.
		 */
		int windowSize() const;

		/**
		 * Sets the window size (both width and height) of the kernel.
		 * @param iValue Integer with the window size in pixels. If the
		 * provided value is even, 1 will be added to make it odd (since
		 * the kernel has its origin at the center). The minimum value
		 * accepted is 3.
		 */
		void setWindowSize(int iValue);

		/**
		 * Gets the bandwidth of the signal(s) that will respond to the kernel,
		 * calculated from the ration between sigma and lambda (for further
		 * information, refer to: http://www.cs.rug.nl/~imaging/simplecell.html#sigma).
		 * @return Double with the bandwidth of the signal that will respond
		 * to the kernel.
		 */
		double bandWidth() const;

		/**
		 * Enumeration defining the different components of the Gabor kernel.
		 */
		enum KernelComponent
		{
			/** The real component of the Gabor kernel. */
			RealComp,

			/** The imaginary component of the Gabor kernel. */
			ImaginaryComp
		};

		/**
		 * Gets the OpenCV's Mat with the kernel data of the given component.
		 * @param eComp Value of the KernelComponent enumeration with the component
		 * for which to get the data. The default is RealComp (i.e. the real component).
		 * @return OpenCV's Mat with the kernel values for the requested component.
		 */
		cv::Mat data(const KernelComponent eComp = RealComp);

		/**
		 * Gets the thumbnail of the Gabor kernel, normalized to gray scale so it can be
		 * used for visual inspection of the kernel values.
		 * @param eComp Value of the KernelComponent enumeration with the component to
		 * create the thumbnail for. The default is RealComp (i.e. the real component).
		 * @param oSize OpenCV Size struct with the size to create the thumbnail. The
		 * default is Size(64, 64).
		 * @param bResize Boolean indicating if the thumbnail should be resized to oSize
		 * or not. If the thumbnail is not resized, it will be cropped and centered at
		 * oSize as needed. The default is true.
		 * @return OpenCV's Mat with the thumbnail image of the kernel values for the
		 * requested component.
		 */
		cv::Mat getThumbnail(const KernelComponent eComp = RealComp, const cv::Size oSize = cv::Size(64,64), const bool bResize = true) const;

		/**
		 * Filters the given image with the kernel and get the responses (that is,
		 * convolve the image with both the real and imaginary components and calculate
		 * the magnitude/energy between their responses).
		 * @param oImage OpenCV's Mat with the image in which to apply the filter.
		 * @param oResponses Reference to an OpenCV's Mat that will receive the filter
		 * responses.
		 */
		void filter(const cv::Mat &oImage, cv::Mat &oResponses);

		/**
		 * Filters the given image with the kernel and get the responses (that is,
		 * convolve the image with both the real and imaginary components and calculate
		 * the magnitude/energy between their responses), as well as the real and 
		 * imaginary components used when filtering.
		 * @param oImage OpenCV's Mat with the image in which to apply the filter.
		 * @param oResponses Reference to an OpenCV's Mat that will receive the filter
		 * responses.
		 * @param oReal Reference to an OpenCV's Mat that will receive the real component
		 * of the responses.
		 * @param oImaginary Reference to an OpenCV's Mat that will receive the imaginary
		 * component of the responses.
		 */
		void filter(const cv::Mat &oImage, cv::Mat &oResponses, cv::Mat &oReal, cv::Mat &oImaginary);

	protected:

		/**
		 * Rebuild the kernel based on the current parameters.
		 */
		void rebuildKernel();

	private:

		/** Orientation (θ) of the sinusoidal carrier of the kernel, in radians. */
		double m_dTheta;

		/** Wavelength (λ) of the sinusoidal carrier of the kernel, in pixels. */
		double m_dLambda;

		/** Phase (ψ) of the sinusoidal carrier of the kernel, in radians. */
		double m_dPsi;

		/** Standard deviation (σ) of the Gaussian envelop of the kernel, in radians. */
		double m_dSigma;

		/** Width and height of the squared kernel window. */
		int m_iWindowSize;

		/** OpenCV's Mat with the real data for the Gabor kernel. */
		cv::Mat m_oRealComp;

		/** OpenCV's Mat with the imaginary data for the Gabor kernel. */
		cv::Mat m_oImaginaryComp;
	};
}

#endif // GABORKERNEL_H