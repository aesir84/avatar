#pragma once

#include "videostream_stereoapp.h"

namespace avatar
{

	class ImageHandler;

	class WebcamStereoApp : public VideoStreamStereoApp
	{

		Q_OBJECT

	public:

		WebcamStereoApp();
		~WebcamStereoApp();

	protected:

		virtual void initializeApp() override;

	private:

		std::array<std::unique_ptr<ImageHandler>, ovrEye_Count> m_imageHandlers;

	};

}
