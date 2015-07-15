#pragma once

#include "videostream_monoapp.h"

namespace avatar
{

	class ImageHandler;

	class WebcamMonoApp : public VideoStreamMonoApp
	{

		Q_OBJECT

	public:

		WebcamMonoApp();

	protected:

		virtual void initializeApp() override;

	private:

		std::unique_ptr<ImageHandler> m_imageHandler;

	};

}
