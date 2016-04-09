#pragma once

#include "videostream_stereoapp.h"

namespace avatar
{

	class FirstPersonViewCli;
	class OVRVisionImageReader;

	class FirstPersonViewApp : public VideoStreamStereoApp
	{

		Q_OBJECT

	public:

		FirstPersonViewApp();

	protected:

		virtual void initializeApp() override final;

	private:

		void rotateFpv(float yaw, float pitch, float roll);

		void raiseError(QString const & description);
		void raiseWarning(QString const & description);

	private:
		enum AngleType
		{
			Yaw,
			Pitch,
			Roll,

			Count
		};

	private:
		float correctAngle(float angleValue, int angleType);

	private:
		float m_previousAngles[Count];
		static float const m_diffThreshold;

	private:

		QTextStream m_stdOutput;

		std::unique_ptr<FirstPersonViewCli> m_commandInterpreter;
		std::unique_ptr<OVRVisionImageReader> m_imageReader;

	};

}
