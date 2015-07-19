#include "stdafx.h"

#include "webcam_monoapp.h"

#include "exception.h"
#include "image_system.h"
#include "image_writer.h"
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

		m_imageHandler = ImageSystem::create(std::make_unique<WebcamImageReader>(cameras[0]),
			                                 std::make_unique<WebcamImageProcessor>());
		QObject::connect(m_imageHandler.get(), &ImageSystem::imageReady, this, &WebcamMonoApp::setEyeImage);

		m_imageHandler->run();
	}

}
