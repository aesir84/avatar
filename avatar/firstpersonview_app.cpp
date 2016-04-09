#include "stdafx.h"

#include "firstpersonview_app.h"

#include "exception.h"
#include "firstpersonview_serialcli.h"
#include "fpv_anglecalc.h"
#include "ovrvision_imagereader.h"

namespace avatar
{
	float const FirstPersonViewApp::m_diffThreshold = static_cast<float>(M_PI);

	FirstPersonViewApp::FirstPersonViewApp()
		: m_stdOutput(stdout)
	{
		m_previousAngles[Yaw] = 0.f;
		m_previousAngles[Pitch] = 0.f;
		m_previousAngles[Roll] = 0.f;
	}

	void FirstPersonViewApp::initializeApp()
	{
		VideoStreamStereoApp::initializeApp();

		QObject::connect(this, &FirstPersonViewApp::orientationChanged, this, &FirstPersonViewApp::rotateFpv);


		//
		// Find usb serial port.
		//

		QList<QSerialPortInfo> const ports = QSerialPortInfo::availablePorts();
		QList<QSerialPortInfo>::const_iterator serialPort = ports.cend();

		for (auto it = ports.cbegin(); it != ports.cend(); ++it)
		{
			QString const description = it->description();

			if (description.contains("arduino", Qt::CaseInsensitive) || description.contains("usb serial", Qt::CaseInsensitive))
			{
				serialPort = it;
				break;
			}
		}

		if (serialPort == ports.cend())
		{
			throw Exception("FPV error", "failed to find USB serial port");
		}

		m_commandInterpreter = avatar::FirstPersonViewSerialCli::create(*serialPort, QSerialPort::Baud115200, QByteArray("Servo>"), 50);
		m_commandInterpreter->initialize();


		//
		// Setup OVRVision reader.
		//

		bool const visionOpened = m_vision.Open(0, OVR::OV_CAMVR_FULL) > 0;

		if (!visionOpened)
		{
			throw Exception("FPV error", "failed to open OVRVision camera");
		}

		int const imageWidth = m_vision.GetCamWidth();
		int const imageHeight = m_vision.GetCamHeight();

		m_leftImage = std::make_shared<cv::Mat>(imageHeight, imageWidth, CV_8UC4);
		m_rightImage = std::make_shared<cv::Mat>(imageHeight, imageWidth, CV_8UC4);
	}

	void FirstPersonViewApp::startFrame()
	{
		VideoStreamStereoApp::startFrame();

		m_vision.PreStoreCamData(OVR::OV_CAMQT_DMS);

		m_vision.GetCamImageBGRA(m_leftImage->data, OVR::OV_CAMEYE_LEFT);
		m_vision.GetCamImageBGRA(m_rightImage->data, OVR::OV_CAMEYE_RIGHT);

		cv::flip(m_leftImage->clone(), *m_leftImage, 0);
		cv::flip(m_rightImage->clone(), *m_rightImage, 0);

		setLeftEyeImage(m_leftImage);
		setRightEyeImage(m_rightImage);
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
