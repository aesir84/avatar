#include "stdafx.h"

#include "webcam_monoapp.h"

#include "exception.h"
#include "image_handler.h"
#include "image_processor.h"
#include "webcam_imagereader.h"

namespace avatar
{

	WebcamMonoApp::WebcamMonoApp()
	{}

	void WebcamMonoApp::initializeApp()
	{
		VideoStreamMonoApp::initializeApp();

		auto const cameras = QCameraInfo::availableCameras();

		if (cameras.empty())
		{
			throw Exception("Webcam error", "no available devices found");
		}

		auto webcamImageReader = std::make_unique<WebcamImageReader>(cameras[0]);
		auto webcamImageProcessor = std::make_unique<RightWebcamImageProcessor>();

		m_imageHandler = std::make_unique<ImageHandler>(std::move(webcamImageReader), std::move(webcamImageProcessor));

		QObject::connect(m_imageHandler.get(), &ImageHandler::imageReady, this, &WebcamMonoApp::setEyeImage);

		m_imageHandler->startHandling();
	}

}
