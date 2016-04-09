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
		// Setup image systems for both eyes.
		//

		// Setup left eye image system.
		//
		m_imageSystems[ovrEye_Left] = ImageSystem::create(std::make_unique<NetworkImageReader>(getLeftEyePort()));
		QObject::connect(m_imageSystems[ovrEye_Left].get(), &ImageSystem::imageReady, this, static_cast<void (VideoStreamServerApp::*)(ImagePtr)>(&VideoStreamServerApp::setLeftEyeImage));
		QObject::connect(m_imageSystems[ovrEye_Left].get(), &ImageSystem::errorOccured, this, &VideoStreamServerApp::raiseVideoStreamException);

		// Setup right eye image system.
		//
		m_imageSystems[ovrEye_Right] = ImageSystem::create(std::make_unique<NetworkImageReader>(getRightEyePort()));
		QObject::connect(m_imageSystems[ovrEye_Right].get(), &ImageSystem::imageReady, this, static_cast<void (VideoStreamServerApp::*)(ImagePtr)>(&VideoStreamServerApp::setRightEyeImage));
		QObject::connect(m_imageSystems[ovrEye_Right].get(), &ImageSystem::errorOccured, this, &VideoStreamServerApp::raiseVideoStreamException);


		//
		// Start the handlers' work.
		//

		m_imageSystems[ovrEye_Left]->run();
		m_imageSystems[ovrEye_Right]->run();
	}

}
