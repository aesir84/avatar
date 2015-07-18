#include "stdafx.h"

#include "videostream_serverapp.h"

#include "image_handler.h"
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
		m_imageHandlers[ovrEye_Left] = std::make_unique<ImageHandler>(std::make_unique<NetworkImageReader>(getLeftEyePort()));
		QObject::connect(m_imageHandlers[ovrEye_Left].get(), &ImageHandler::imageReady, this, &VideoStreamServerApp::setLeftEyeImage);

		// Setup right eye image handler.
		//
		m_imageHandlers[ovrEye_Right] = std::make_unique<ImageHandler>(std::make_unique<NetworkImageReader>(getRightEyePort()));
		QObject::connect(m_imageHandlers[ovrEye_Right].get(), &ImageHandler::imageReady, this, &VideoStreamServerApp::setRightEyeImage);


		//
		// Start the handlers' work.
		//

		m_imageHandlers[ovrEye_Left]->startHandling();
		m_imageHandlers[ovrEye_Right]->startHandling();
	}

}
