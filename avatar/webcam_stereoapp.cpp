#include "stdafx.h"

#include "webcam_stereoapp.h"

#include "exception.h"
#include "image_handler.h"
#include "image_processor.h"
#include "webcam_imagereader.h"

namespace avatar
{

	WebcamStereoApp::WebcamStereoApp()
	{}

	WebcamStereoApp::~WebcamStereoApp()
	{}

	void WebcamStereoApp::initializeApp()
	{
		VideoStreamStereoApp::initializeApp();


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
		m_imageHandlers[ovrEye_Left] = std::make_unique<ImageHandler>(std::make_unique<WebcamImageReader>(cameras[1]),
			                                                          std::make_unique<LeftWebcamImageProcessor>());
		QObject::connect(m_imageHandlers[ovrEye_Left].get(), &ImageHandler::imageReady, this, &WebcamStereoApp::setLeftEyeImage);

		// Setup right eye image handler.
		//
		m_imageHandlers[ovrEye_Right] = std::make_unique<ImageHandler>(std::make_unique<WebcamImageReader>(cameras[0]),
			                                                           std::make_unique<RightWebcamImageProcessor>());
		QObject::connect(m_imageHandlers[ovrEye_Right].get(), &ImageHandler::imageReady, this, &WebcamStereoApp::setRightEyeImage);


		//
		// Start the handlers' work.
		//

		m_imageHandlers[ovrEye_Left]->startHandling();
		m_imageHandlers[ovrEye_Right]->startHandling();
	}

}
