#pragma once

#include "imagesystem_module.h"

namespace avatar
{

	class WebcamImageProcessor : public ImageSystemModule
	{

		Q_OBJECT

	public:

		enum class WebcamSide
		{
			Left, Right
		};

	public:

		explicit WebcamImageProcessor(WebcamSide webcamSide = WebcamSide::Right);

	public:

		virtual ModuleOperation getOperationType() const override;

	private:

		virtual void initialize() override;
		virtual void processImage(ImagePtr image) override;

	private:

		QTransform m_rotation;

	};

}
