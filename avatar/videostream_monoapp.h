#pragma once

#include "imagesystem_typedefs.h"
#include "videostream_app.h"

namespace avatar
{

	class VideoStreamMonoApp : public VideoStreamApp
	{

		Q_OBJECT

	public:

		VideoStreamMonoApp();
		~VideoStreamMonoApp();

	protected:

		virtual void initializeApp() override;

		virtual void bindEyeTexture(ovrEyeType renderedEye) override final;
		virtual void releaseEyeTexture(ovrEyeType renderedEye) override final;

	protected:

		void setEyeImage(ImagePtr image);

	private:

		std::unique_ptr<QOpenGLTexture> m_eyeTexture;

		size_t m_currentFramesPerImageCount;
		std::vector<size_t> m_framesPerImages;

	};

}
