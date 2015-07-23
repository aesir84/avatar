#pragma once

#include "imagesystem_module.h"

namespace avatar
{

	class WebcamImageReader : public ImageSystemModule
	{

		Q_OBJECT

	public:

		explicit WebcamImageReader(QCameraInfo const & cameraInfo);

	public:

		virtual ModuleOperation getOperationType() const override;

	private:

		virtual void initialize() override;
		virtual void processImage(ImagePtr image) override;

	private:

		static std::mutex m_cameraSetupMutex;

	private:

		QCameraInfo m_cameraInfo;

	};

}
