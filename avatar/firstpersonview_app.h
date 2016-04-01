#pragma once

#include "videostream_monoapp.h"

namespace avatar
{

	class FirstPersonViewCli;
	class ImageSystem;

	class FirstPersonViewApp : public VideoStreamMonoApp
	{

		Q_OBJECT

	public:

		FirstPersonViewApp(std::unique_ptr<FirstPersonViewCli> && commandInterpreter, std::unique_ptr<ImageSystem> && imageSystem);

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
		std::unique_ptr<ImageSystem> m_imageSystem;

	};

}
