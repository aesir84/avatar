#include "stdafx.h"

#include "videostream_clientapp.h"

#include "exception.h"
#include "image_handler.h"
#include "image_processor.h"
#include "network_imagewriter.h"
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
		auto leftWebcamImageReader = std::make_unique<WebcamImageReader>(cameras[1]);
		auto leftWebcamImageProcessor = std::make_unique<LeftWebcamImageProcessor>();
		auto leftWebcamImageWriter = NetworkImageWriter::create(m_serverAddress, getLeftEyePort());

		m_imageHandlers[ovrEye_Left] = std::make_unique<ImageHandler>(std::move(leftWebcamImageReader), std::move(leftWebcamImageProcessor), std::move(leftWebcamImageWriter));

		QObject::connect(m_imageHandlers[ovrEye_Left].get(), &ImageHandler::imageReady, this, &VideoStreamClientApp::setLeftEyeImage);

		// Setup right eye image handler.
		//
		auto rightWebcamImageReader = std::make_unique<WebcamImageReader>(cameras[0]);
		auto rightWebcamImageProcessor = std::make_unique<RightWebcamImageProcessor>();
		auto rightWebcamImageWriter = NetworkImageWriter::create(m_serverAddress, getRightEyePort());

		m_imageHandlers[ovrEye_Right] = std::make_unique<ImageHandler>(std::move(rightWebcamImageReader), std::move(rightWebcamImageProcessor), std::move(rightWebcamImageWriter));

		QObject::connect(m_imageHandlers[ovrEye_Right].get(), &ImageHandler::imageReady, this, &VideoStreamClientApp::setRightEyeImage);

		//
		// Start the handlers' work.
		//

		m_imageHandlers[ovrEye_Left]->startHandling();
		m_imageHandlers[ovrEye_Right]->startHandling();
	}

}
