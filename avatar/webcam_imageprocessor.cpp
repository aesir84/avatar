#include "stdafx.h"

#include "webcam_imageprocessor.h"

namespace avatar
{

	WebcamImageProcessor::WebcamImageProcessor(WebcamSide webcamSide)
	{
		switch (webcamSide)
		{
			case WebcamSide::Left:
			{
				m_rotation.rotate(-90.0);
			}
			break;

			case WebcamSide::Right:
			{
				m_rotation.rotate(90.0);
			}
			break;

			default:
			{
				Q_ASSERT(false);
			}
		}
	}

	void WebcamImageProcessor::processImage(ImagePtr image)
	{
		auto processedImage = image->transformed(m_rotation);
		image->swap(processedImage);

		emit imageProcessed(image);
	}

}
