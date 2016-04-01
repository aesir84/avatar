#include "stdafx.h"

#include "firstpersonview_app.h"

#include "exception.h"
#include "firstpersonview_cli.h"
#include "image_system.h"
#include "fpv_anglecalc.h"

namespace avatar
{
	float const FirstPersonViewApp::m_diffThreshold = static_cast<float>(M_PI);

	FirstPersonViewApp::FirstPersonViewApp(std::unique_ptr<FirstPersonViewCli> && commandInterpreter, std::unique_ptr<ImageSystem> && imageSystem)
		: m_commandInterpreter(std::move(commandInterpreter))
		, m_imageSystem(std::move(imageSystem))
		, m_stdOutput(stdout)
	{
		m_previousAngles[Yaw] = 0.f;
		m_previousAngles[Pitch] = 0.f;
		m_previousAngles[Roll] = 0.f;
	}

	void FirstPersonViewApp::initializeApp()
	{
		VideoStreamMonoApp::initializeApp();

		QObject::connect(m_imageSystem.get(), &ImageSystem::imageReady, this, &FirstPersonViewApp::setEyeImage);
		QObject::connect(m_imageSystem.get(), &ImageSystem::errorOccured, this, &FirstPersonViewApp::raiseVideoStreamException);
		QObject::connect(this, &FirstPersonViewApp::orientationChanged, this, &FirstPersonViewApp::rotateFpv);

		m_commandInterpreter->initialize();
		m_imageSystem->run();
	}

	void FirstPersonViewApp::rotateFpv(float yaw, float pitch, float roll)
	{
		auto const correctedYaw = correctAngle(yaw, Yaw);
		auto const correctedPitch = correctAngle(pitch, Pitch);
		auto const correctedRoll = correctAngle(roll, Roll);

		auto const command = FirstPersonViewCli::createCommand(fpv_utils::TwoAxisFpvAngleCalculator(), correctedYaw, correctedPitch, correctedRoll);
		m_commandInterpreter->sendCommand(command);
	}

	void FirstPersonViewApp::raiseError(QString const & description)
	{
		throw Exception("FPV error", description);
	}

	void FirstPersonViewApp::raiseWarning(QString const & description)
	{
		m_stdOutput << description << endl;
	}

	float FirstPersonViewApp::correctAngle(float angleValue, int angleType)
	{
		float const diff = abs(angleValue - m_previousAngles[angleType]);

		float correctedAngleValue = angleValue;
		if (diff > m_diffThreshold)
		{
			correctedAngleValue = m_previousAngles[angleType];
		}
		else
		{
			m_previousAngles[angleType] = correctedAngleValue;
		}

		return correctedAngleValue;
	}

}
