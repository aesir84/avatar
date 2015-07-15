#include "stdafx.h"

#include "image_processor.h"

namespace avatar
{

	ImageProcessor::~ImageProcessor()
	{}

	LeftWebcamImageProcessor::LeftWebcamImageProcessor()
		: m_rotation(QTransform().rotate(-90.0))
	{}

	void LeftWebcamImageProcessor::processImage(ImagePtr image)
	{
		auto processedImage = image->transformed(m_rotation);
		image->swap(processedImage);

		emit imageProcessed(image);
	}

	RightWebcamImageProcessor::RightWebcamImageProcessor()
		: m_rotation(QTransform().rotate(90.0))
	{}

	void RightWebcamImageProcessor::processImage(ImagePtr image)
	{
		auto processedImage = image->transformed(m_rotation);
		image->swap(processedImage);

		emit imageProcessed(image);
	}

}
