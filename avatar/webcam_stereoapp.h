#pragma once

#include "videostream_stereoapp.h"

namespace avatar
{

	class ImageSystem;

	class WebcamStereoApp : public VideoStreamStereoApp
	{

		Q_OBJECT

	public:

		WebcamStereoApp();
		~WebcamStereoApp();

	protected:

		virtual void initializeApp() override;

	private:

		std::array<std::unique_ptr<ImageSystem>, ovrEye_Count> m_imageHandlers;

	};

}
