#pragma once

#include "image_typedef.h"

namespace avatar
{

	class ImageProcessor : public QObject
	{

		Q_OBJECT

	public:

		virtual ~ImageProcessor();

	public slots:

		virtual void processImage(ImagePtr image) = 0;

	signals:

		void imageProcessed(ImagePtr image);

	};

	class LeftWebcamImageProcessor : public ImageProcessor
	{

		Q_OBJECT

	public:

		LeftWebcamImageProcessor();

	public slots:

		virtual void processImage(ImagePtr image) override;

	private:

		QTransform m_rotation;

	};

	class RightWebcamImageProcessor : public ImageProcessor
	{

		Q_OBJECT

	public:

		RightWebcamImageProcessor();

	public slots:

		virtual void processImage(ImagePtr image) override;

	private:

		QTransform m_rotation;

	};

}
