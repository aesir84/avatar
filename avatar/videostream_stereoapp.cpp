#include "stdafx.h"

#include "videostream_stereoapp.h"

namespace avatar
{

	VideoStreamStereoApp::VideoStreamStereoApp()
	{}

	void VideoStreamStereoApp::initializeApp()
	{
		VideoStreamApp::initializeApp();
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
		}
	}

}
