#include "stdafx.h"

#include "webcam_stereoapp.h"

#include "exception.h"
#include "image_system.h"
#include "webcam_imageprocessor.h"
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
		m_imageHandlers[ovrEye_Left] = ImageSystem::create(std::make_unique<WebcamImageReader>(cameras[1]),
			                                               std::make_unique<WebcamImageProcessor>(WebcamImageProcessor::WebcamSide::Left));
		QObject::connect(m_imageHandlers[ovrEye_Left].get(), &ImageSystem::imageReady, this, &WebcamStereoApp::setLeftEyeImage);

		// Setup right eye image handler.
		//
		m_imageHandlers[ovrEye_Right] = ImageSystem::create(std::make_unique<WebcamImageReader>(cameras[0]),
			                                                std::make_unique<WebcamImageProcessor>(WebcamImageProcessor::WebcamSide::Right));
		QObject::connect(m_imageHandlers[ovrEye_Right].get(), &ImageSystem::imageReady, this, &WebcamStereoApp::setRightEyeImage);


		//
		// Start the handlers' work.
		//

		m_imageHandlers[ovrEye_Left]->run();
		m_imageHandlers[ovrEye_Right]->run();
	}

}
