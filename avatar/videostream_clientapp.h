#pragma once

#include "videostream_networkapp.h"

namespace avatar
{

	class ImageSystem;

	class VideoStreamClientApp : public VideoStreamNetworkApp
	{

		Q_OBJECT

	public:

		VideoStreamClientApp(QHostAddress const & serverAddress);

	protected:

		virtual void initializeApp() override;

	private:

		QHostAddress m_serverAddress;
		std::array<std::unique_ptr<ImageSystem>, ovrEye_Count> m_imageSystems;

	};

}
