#include "stdafx.h"

#include "webcam_monoapp.h"

#include "exception.h"
#include "image_system.h"
#include "webcam_imageprocessor.h"
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

		m_imageSystem = ImageSystem::create(std::make_unique<WebcamImageReader>(cameras[0]),
			                                 std::make_unique<WebcamImageProcessor>());
		QObject::connect(m_imageSystem.get(), &ImageSystem::imageReady, this, &WebcamMonoApp::setEyeImage);
		QObject::connect(m_imageSystem.get(), &ImageSystem::errorOccured, this, &WebcamMonoApp::raiseVideoStreamException);

		m_imageSystem->run();
	}

}
