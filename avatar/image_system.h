#pragma once

#include "image_typedef.h"

namespace avatar
{

	class ImageProcessor;
	class ImageReader;
	class ImageWriter;

	/// \brief A system that manages the image processing pipeline
	///
	/// The class represents a system, that binds together all the pieces
	/// of the image processing pipeline, like reading, processing and writing.
	/// The class must have exclusive access to the initialization means of classes,
	/// that make up the pipeline, i.e. ImageSystem must be a friend of e.g. ImageReader class,
	/// while ImageReader class must keep its initialize() method private to support the scheme.
	///
	class ImageSystem : public QObject
	{

		Q_OBJECT

	public:

		ImageSystem(ImageSystem const & other) = delete;
		ImageSystem & operator=(ImageSystem const & rhs) = delete;

		static std::unique_ptr<ImageSystem> create(std::unique_ptr<ImageReader> && imageReader,
			                                       std::unique_ptr<ImageProcessor> && imageProcessor = nullptr,
												   std::unique_ptr<ImageWriter> && imageWriter = nullptr);

		void run();

	Q_SIGNALS:

		void imageReady(ImagePtr image);

	private:

		ImageSystem(std::unique_ptr<ImageReader> && imageReader,
			        std::unique_ptr<ImageProcessor> && imageProcessor = nullptr,
			        std::unique_ptr<ImageWriter> && imageWriter = nullptr);

	private:

		std::unique_ptr<ImageReader> m_imageReader;
		std::unique_ptr<ImageProcessor> m_imageProcessor;
		std::unique_ptr<ImageWriter> m_imageWriter;

	};

}
