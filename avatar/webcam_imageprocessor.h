#pragma once

#include "image_processor.h"

namespace avatar
{

	class WebcamImageProcessor : public ImageProcessor
	{

		Q_OBJECT

	public:

		enum class WebcamSide
		{
			Left, Right
		};

	public:

		explicit WebcamImageProcessor(WebcamSide webcamSide = WebcamSide::Right);

	private:

		virtual void processImage(ImagePtr image) override;

	private:

		QTransform m_rotation;

	};

}
