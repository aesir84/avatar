#include "stdafx.h"

#include "videostream_networkapp.h"

#include "network_imagereader.h"

namespace avatar
{

	VideoStreamNetworkApp::VideoStreamNetworkApp()
	{}

	void VideoStreamNetworkApp::initializeApp()
	{
		VideoStreamStereoApp::initializeApp();
	}

	quint16 VideoStreamNetworkApp::getLeftEyePort() const
	{
		return m_leftEyePort;
	}

	quint16 VideoStreamNetworkApp::getRightEyePort() const
	{
		return m_rightEyePort;
	}

}
