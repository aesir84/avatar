#include "stdafx.h"

#include "firstpersonview_app.h"

#include "exception.h"
#include "firstpersonview_cli.h"
#include "image_system.h"
#include "fpv_anglecalc.h"

namespace avatar
{

	FirstPersonViewApp::FirstPersonViewApp(std::unique_ptr<FirstPersonViewCli> && commandInterpreter, std::unique_ptr<ImageSystem> && imageSystem)
		: m_commandInterpreter(std::move(commandInterpreter))
		, m_imageSystem(std::move(imageSystem))
		, m_stdOutput(stdout)
	{ }

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
		auto const command = FirstPersonViewCli::createCommand(fpv_utils::TwoAxisFpvAngleCalculator(), yaw, pitch, roll);
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

}
