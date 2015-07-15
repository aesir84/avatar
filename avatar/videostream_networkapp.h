#pragma once

#include "videostream_stereoapp.h"

namespace avatar
{

	class ImageHandler;

	class VideoStreamNetworkApp : public VideoStreamStereoApp
	{

		Q_OBJECT

	public:

		VideoStreamNetworkApp();

	protected:

		virtual void initializeApp() override;

	protected:

		quint16 getLeftEyePort() const;
		quint16 getRightEyePort() const;

	private:

		static quint16 const m_leftEyePort = 7710;
		static quint16 const m_rightEyePort = 7701;

	};

}
