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
		// Setup image systems for both eyes.
		//

		auto const cameras = QCameraInfo::availableCameras();

		if (cameras.size() < 2)
		{
			throw Exception("Webcam error", "no available devices found");
		}

		// Setup left eye image system.
		//
		m_imageSystems[ovrEye_Left] = ImageSystem::create(std::make_unique<WebcamImageReader>(cameras[1]),
			                                              std::make_unique<WebcamImageProcessor>(WebcamImageProcessor::WebcamSide::Left));
		QObject::connect(m_imageSystems[ovrEye_Left].get(), &ImageSystem::imageReady, this, static_cast<void (WebcamStereoApp::*)(ImagePtr)>(&WebcamStereoApp::setLeftEyeImage));
		QObject::connect(m_imageSystems[ovrEye_Left].get(), &ImageSystem::errorOccured, this, &WebcamStereoApp::raiseVideoStreamException);

		// Setup right eye image system.
		//
		m_imageSystems[ovrEye_Right] = ImageSystem::create(std::make_unique<WebcamImageReader>(cameras[0]),
			                                               std::make_unique<WebcamImageProcessor>(WebcamImageProcessor::WebcamSide::Right));
		QObject::connect(m_imageSystems[ovrEye_Right].get(), &ImageSystem::imageReady, this, static_cast<void (WebcamStereoApp::*)(ImagePtr)>(&WebcamStereoApp::setRightEyeImage));
		QObject::connect(m_imageSystems[ovrEye_Right].get(), &ImageSystem::errorOccured, this, &WebcamStereoApp::raiseVideoStreamException);


		//
		// Start the handlers' work.
		//

		m_imageSystems[ovrEye_Left]->run();
		m_imageSystems[ovrEye_Right]->run();
	}

}
