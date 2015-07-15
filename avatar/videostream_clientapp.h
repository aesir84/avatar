#pragma once

#include "videostream_networkapp.h"

namespace avatar
{

	class ImageHandler;

	class VideoStreamClientApp : public VideoStreamNetworkApp
	{

		Q_OBJECT

	public:

		VideoStreamClientApp(QHostAddress const & serverAddress);

	protected:

		virtual void initializeApp() override;

	private:

		QHostAddress m_serverAddress;
		std::array<std::unique_ptr<ImageHandler>, ovrEye_Count> m_imageHandlers;

	};

}
