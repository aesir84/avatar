#include "stdafx.h"

#include "videostream_clientapp.h"

#include "exception.h"
#include "image_system.h"
#include "network_imagewriter.h"
#include "webcam_imageprocessor.h"
#include "webcam_imagereader.h"

namespace avatar
{

	VideoStreamClientApp::VideoStreamClientApp(QHostAddress const & serverAddress)
		: m_serverAddress(serverAddress)
	{}

	void VideoStreamClientApp::initializeApp()
	{
		VideoStreamNetworkApp::initializeApp();


		//
		// Setup image handlers for both eyes.
		//

		auto const cameras = QCameraInfo::availableCameras();

		if (cameras.size() < 2)
		{
			throw Exception("Webcam error", "no available devices found");
		}

		// Setup left eye image handler.
		//
		m_imageHandlers[ovrEye_Left] = ImageSystem::create(std::make_unique<WebcamImageReader>(cameras[1]),
			                                               std::make_unique<WebcamImageProcessor>(WebcamImageProcessor::WebcamSide::Left),
														   NetworkImageWriter::create(m_serverAddress, getLeftEyePort()));
		QObject::connect(m_imageHandlers[ovrEye_Left].get(), &ImageSystem::imageReady, this, &VideoStreamClientApp::setLeftEyeImage);

		// Setup right eye image handler.
		//
		m_imageHandlers[ovrEye_Right] = ImageSystem::create(std::make_unique<WebcamImageReader>(cameras[0]),
			                                                std::make_unique<WebcamImageProcessor>(WebcamImageProcessor::WebcamSide::Right),
															NetworkImageWriter::create(m_serverAddress, getRightEyePort()));
		QObject::connect(m_imageHandlers[ovrEye_Right].get(), &ImageSystem::imageReady, this, &VideoStreamClientApp::setRightEyeImage);


		//
		// Start the handlers' work.
		//

		m_imageHandlers[ovrEye_Left]->run();
		m_imageHandlers[ovrEye_Right]->run();
	}

}
