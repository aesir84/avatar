#pragma once

#include "image_typedef.h"
#include "videostream_app.h"

namespace avatar
{

	class VideoStreamMonoApp : public VideoStreamApp
	{

		Q_OBJECT

	public:

		VideoStreamMonoApp();

	protected:

		virtual void initializeApp() override;

		virtual void bindEyeTexture(ovrEyeType renderedEye) override final;
		virtual void releaseEyeTexture(ovrEyeType renderedEye) override final;

	protected slots:

		void setEyeImage(ImagePtr image);

	private:

		std::unique_ptr<QOpenGLTexture> m_eyeTexture;

	};

}
