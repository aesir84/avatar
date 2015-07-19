#include "stdafx.h"

#include "webcam_imagereader.h"

#include "webcam_capture.h"

namespace avatar
{

	std::mutex WebcamImageReader::m_cameraSetupMutex;

	WebcamImageReader::WebcamImageReader(QCameraInfo const & cameraInfo)
		: m_cameraInfo(cameraInfo)
	{}

	void WebcamImageReader::initialize()
	{

		//
		// Setup the camera object.
		//

		// Lock access to camera setup activities.
		//
		std::lock_guard<std::mutex> lock(m_cameraSetupMutex); // being static the mutex affects all objects of this class

		// Use a smart pointer to automatically delete the camera object,
		// if we exit the scope unexpectedly due to a failure.
		// Otherwise there will be a memory leak.
		//
		std::unique_ptr<QCamera> webcam = std::make_unique<QCamera>(m_cameraInfo, this); // set the parent of the camera, so that it will be destroyed once the destructor of this object is called

		if (webcam->error() != QCamera::NoError)
		{
			emit readerBroken(webcam->errorString());
			return;
		}

		auto webcamCapture = std::make_unique<WebcamCapture>();

		// Connect the image retransmitting signal slot chain.
		//
		QObject::connect(webcamCapture.get(), &WebcamCapture::imageCaptured, this, &WebcamImageReader::imageRead);

		webcam->setViewfinder(webcamCapture.release());
		webcam->start();

		// Let the smartpointer release the ownership of the camera object.
		// Otherwise it will delete the camera when it gets out of scope.
		//
		webcam.release();
	}

}
