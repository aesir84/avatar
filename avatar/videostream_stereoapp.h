#pragma once

#include "imagesystem_typedefs.h"
#include "videostream_app.h"

namespace avatar
{

	class VideoStreamStereoApp : public VideoStreamApp
	{

		Q_OBJECT

	public:

		VideoStreamStereoApp();
		~VideoStreamStereoApp();

	protected:

		virtual void initializeApp() override;

		virtual void bindEyeTexture(ovrEyeType renderedEye) override final;
		virtual void releaseEyeTexture(ovrEyeType renderedEye) override final;

	protected slots:

		void setLeftEyeImage(ImagePtr image);
		void setRightEyeImage(ImagePtr image);

	private:

		void setEyeImage(ovrEyeType eye, ImagePtr image);

	private:

		std::array<std::unique_ptr<QOpenGLTexture>, ovrEye_Count> m_eyeTextures;
		std::array<std::vector<size_t>, ovrEye_Count> m_frameCountsPerEyeImages;
		std::array<size_t, ovrEye_Count> m_currentFrameCounts;

	};

}
