#include "stdafx.h"

#include "videostream_serverapp.h"

#include "image_system.h"
#include "network_imagereader.h"

namespace avatar
{

	VideoStreamServerApp::VideoStreamServerApp()
	{}

	void VideoStreamServerApp::initializeApp()
	{
		VideoStreamNetworkApp::initializeApp();


		//
		// Setup image handlers for both eyes.
		//

		// Setup left eye image handler.
		//
		m_imageHandlers[ovrEye_Left] = std::make_unique<ImageSystem>(std::make_unique<NetworkImageReader>(getLeftEyePort()));
		QObject::connect(m_imageHandlers[ovrEye_Left].get(), &ImageSystem::imageReady, this, &VideoStreamServerApp::setLeftEyeImage);

		// Setup right eye image handler.
		//
		m_imageHandlers[ovrEye_Right] = std::make_unique<ImageSystem>(std::make_unique<NetworkImageReader>(getRightEyePort()));
		QObject::connect(m_imageHandlers[ovrEye_Right].get(), &ImageSystem::imageReady, this, &VideoStreamServerApp::setRightEyeImage);


		//
		// Start the handlers' work.
		//

		m_imageHandlers[ovrEye_Left]->startHandling();
		m_imageHandlers[ovrEye_Right]->startHandling();
	}

}
