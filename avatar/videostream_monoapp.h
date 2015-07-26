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
		virtual void startFrame() override;

		virtual void bindEyeTexture(ovrEyeType renderedEye) override final;
		virtual void releaseEyeTexture(ovrEyeType renderedEye) override final;

	protected:

		void setEyeImage(ImagePtr image);

	private:

		std::unique_ptr<QOpenGLTexture> m_eyeTexture;

		/// \brief Statistics about image latency
		///
		/// Considering the fact, that the images are coming from possibly high latency devices,
		/// such as webcams or network sockets,
		/// it is worth measuring how many frames use the same single image.
		///
		size_t m_framesCounter;
		std::vector<size_t> m_framesCountPerImages;

	};

}
