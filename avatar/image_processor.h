#pragma once

#include "image_typedef.h"

namespace avatar
{

	class ImageProcessor : public QObject
	{

		Q_OBJECT

	public:

		virtual ~ImageProcessor();

	Q_SIGNALS:

		void imageProcessed(ImagePtr image);

	private:

		friend class ImageSystem;

		virtual void processImage(ImagePtr image) = 0;

	};

}
