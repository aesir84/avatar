#include "stdafx.h"

#include "videostream_monoapp.h"

namespace avatar
{

	VideoStreamMonoApp::VideoStreamMonoApp()
		: m_framesCounter(0)
	{ }

	VideoStreamMonoApp::~VideoStreamMonoApp()
	{
		if (!m_framesCountPerImages.empty())
		{
			std::wcout
				<< L"Average frame count per one image is "
				<< std::accumulate(m_framesCountPerImages.begin(), m_framesCountPerImages.end(), 0) / m_framesCountPerImages.size()
				<< L" frames/image"
				<< std::endl;
		}
	}

	void VideoStreamMonoApp::initializeApp()
	{
		VideoStreamApp::initializeApp();
	}

	void VideoStreamMonoApp::startFrame()
	{
		VideoStreamApp::startFrame();
		++m_framesCounter;
	}

	void VideoStreamMonoApp::bindEyeTexture(ovrEyeType renderedEye)
	{
		Q_UNUSED(renderedEye);

		if (m_eyeTexture != nullptr && !m_eyeTexture->isBound()) m_eyeTexture->bind();
	}

	void VideoStreamMonoApp::releaseEyeTexture(ovrEyeType renderedEye)
	{
		Q_UNUSED(renderedEye);

		if (m_eyeTexture != nullptr && m_eyeTexture->isBound()) m_eyeTexture->release();
	}

	void VideoStreamMonoApp::setEyeImage(ImagePtr image)
	{

		//
		// Convert the image to a format which is used by OpenGL textures in Qt.
		// RGBA8888 is a 32-bit byte-ordered RGBA format (8-8-8-8).
		// This byte-by-byte interpretation suits OpenGL textures.
		//

		auto const textureImage = image->convertToFormat(QImage::Format_RGBA8888);

		//
		// Either create a new texture or refill the already created one.
		//

		if (m_eyeTexture == nullptr)
		{
			// Create a texture from the supplied image.
			//
			m_eyeTexture = std::make_unique<QOpenGLTexture>(textureImage);
		}
		else
		{
			// Update the pixels of the existing texture.
			//
			m_eyeTexture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, textureImage.bits());

			// Update image latency statictics.
			//
			m_framesCountPerImages.push_back(m_framesCounter);
			m_framesCounter = 0;
		}
	}

}
