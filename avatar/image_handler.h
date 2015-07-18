#pragma once

#include "image_typedef.h"

namespace avatar
{

	class ImageProcessor;
	class ImageReader;
	class ImageWriter;

	class ImageHandler : public QObject
	{

		Q_OBJECT

	public:

		ImageHandler(std::unique_ptr<ImageReader> && imageReader);
		ImageHandler(std::unique_ptr<ImageReader> && imageReader, std::unique_ptr<ImageProcessor> && imageProcessor);
		ImageHandler(std::unique_ptr<ImageReader> && imageReader, std::unique_ptr<ImageProcessor> && imageProcessor, std::unique_ptr<ImageWriter> && imageWriter);

	public slots:

		void startHandling();

	signals:

		void imageReady(ImagePtr image);

	private:

		void setupWorkingProcess(std::unique_ptr<ImageReader> && imageReader) const;
		void setupWorkingProcess(std::unique_ptr<ImageProcessor> && imageProcessor, std::unique_ptr<ImageReader> && imageReader) const;
		void setupWorkingProcess(std::unique_ptr<ImageProcessor> && imageProcessor, std::unique_ptr<ImageWriter> && imageWriter, std::unique_ptr<ImageReader> && imageReader) const;

	private:

		std::unique_ptr<ImageReader> m_imageReader;
		std::unique_ptr<ImageProcessor> m_imageProcessor;
		std::unique_ptr<ImageWriter> m_imageWriter;

	};

}
