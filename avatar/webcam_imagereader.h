#pragma once

#include "image_reader.h"

namespace avatar
{

	class WebcamImageReader : public ImageReader
	{

		Q_OBJECT

	public:

		explicit WebcamImageReader(QCameraInfo const & cameraInfo);

	public slots:

		virtual void startReading() override;

	private:

		static std::mutex m_cameraSetupMutex;

	private:

		QCameraInfo m_cameraInfo;

	};

}
