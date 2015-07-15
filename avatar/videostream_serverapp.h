#pragma once

#include "videostream_networkapp.h"

namespace avatar
{

	class ImageHandler;

	class VideoStreamServerApp : public VideoStreamNetworkApp
	{

		Q_OBJECT

	public:

		VideoStreamServerApp();

	protected:

		virtual void initializeApp() override;

	private:

		std::array<std::unique_ptr<ImageHandler>, ovrEye_Count> m_imageHandlers;

	};

}
