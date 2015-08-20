#pragma once

#include "videostream_monoapp.h"

namespace avatar
{

	class ImageSystem;

	class WebcamMonoApp : public VideoStreamMonoApp
	{

		Q_OBJECT

	public:

		WebcamMonoApp();

	protected:

		virtual void initializeApp() override final;

	private:

		std::unique_ptr<ImageSystem> m_imageSystem;

	};

}
