#pragma once

#include "videostream_networkapp.h"

namespace avatar
{

	class ImageSystem;

	class VideoStreamServerApp : public VideoStreamNetworkApp
	{

		Q_OBJECT

	public:

		VideoStreamServerApp();

	protected:

		virtual void initializeApp() override;

	private:

		std::array<std::unique_ptr<ImageSystem>, ovrEye_Count> m_imageSystems;

	};

}
