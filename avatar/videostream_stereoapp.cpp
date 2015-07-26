#include "stdafx.h"

#include "videostream_stereoapp.h"

namespace avatar
{

	VideoStreamStereoApp::VideoStreamStereoApp()
		: m_framesCounters()
	{ }

	VideoStreamStereoApp::~VideoStreamStereoApp()
	{
		if (!m_framesCountPerEyeImages[ovrEye_Left].empty())
		{
			auto const & framesCountPerLeftEyeImages = m_framesCountPerEyeImages[ovrEye_Left];
			std::wcout
				<< L"Average frame count per one image for left eye is "
				<< std::accumulate(framesCountPerLeftEyeImages.begin(), framesCountPerLeftEyeImages.end(), 0) / framesCountPerLeftEyeImages.size()
				<< L" frames/image" << std::endl;
		}

		if (!m_framesCountPerEyeImages[ovrEye_Right].empty())
		{
			auto const & framesCountPerRightEyeImages = m_framesCountPerEyeImages[ovrEye_Right];
			std::wcout
				<< L"Average frame count per one image for right eye is "
				<< std::accumulate(framesCountPerRightEyeImages.begin(), framesCountPerRightEyeImages.end(), 0) / framesCountPerRightEyeImages.size()
				<< L" frames/image" << std::endl;
		}
	}

	void VideoStreamStereoApp::initializeApp()
	{
		VideoStreamApp::initializeApp();
	}

	void VideoStreamStereoApp::startFrame()
	{
		VideoStreamApp::startFrame();

		// Increment the frames count for both eyes.
		//
		++m_framesCounters[ovrEye_Left];
		++m_framesCounters[ovrEye_Right];
	}

	void VideoStreamStereoApp::bindEyeTexture(ovrEyeType renderedEye)
	{
		if (m_eyeTextures[renderedEye] != nullptr && !m_eyeTextures[renderedEye]->isBound()) m_eyeTextures[renderedEye]->bind();
	}

	void VideoStreamStereoApp::releaseEyeTexture(ovrEyeType renderedEye)
	{
		if (m_eyeTextures[renderedEye] != nullptr && m_eyeTextures[renderedEye]->isBound()) m_eyeTextures[renderedEye]->release();
	}

	void VideoStreamStereoApp::setLeftEyeImage(ImagePtr image)
	{
		setEyeImage(ovrEye_Left, image);
	}

	void VideoStreamStereoApp::setRightEyeImage(ImagePtr image)
	{
		setEyeImage(ovrEye_Right, image);
	}

	void VideoStreamStereoApp::setEyeImage(ovrEyeType eye, ImagePtr image)
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

		if (m_eyeTextures[eye] == nullptr)
		{
			// Create a texture from the supplied image.
			//
			m_eyeTextures[eye] = std::make_unique<QOpenGLTexture>(textureImage);
		}
		else
		{
			// Update the pixels of the existing texture.
			//
			m_eyeTextures[eye]->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, textureImage.bits());

			// Update image latency statictics.
			// In this case a certain image counter is handled - the one, that corresponds to the currently handled eye.
			//
			m_framesCountPerEyeImages[eye].push_back(m_framesCounters[eye]);
			m_framesCounters[eye] = 0;
		}
	}

}
