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
		virtual void startFrame() override;

		virtual void bindEyeTexture(ovrEyeType renderedEye) override final;
		virtual void releaseEyeTexture(ovrEyeType renderedEye) override final;

	protected slots:

		void setLeftEyeImage(ImagePtr image);
		void setRightEyeImage(ImagePtr image);

		void setLeftEyeImage(MatPtr image);
		void setRightEyeImage(MatPtr image);

	private:

		void setEyeImage(ovrEyeType eye, ImagePtr image);

		void setEyeImage(ovrEyeType eye, MatPtr image);

	private:

		std::array<std::unique_ptr<QOpenGLTexture>, ovrEye_Count> m_eyeTextures;

		/// \brief Statistics about image latency
		///
		/// Considering the fact, that the images are coming from possibly high latency devices,
		/// such as webcams or network sockets,
		/// it is worth measuring how many frames use the same single image.
		/// In this case there are two images as it is a stereo rig.
		///
		std::array<std::vector<size_t>, ovrEye_Count> m_framesCountPerEyeImages;
		std::array<size_t, ovrEye_Count> m_framesCounters;

	};

}
