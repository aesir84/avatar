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

		m_imageReader = std::make_unique<OVRVisionImageReader>();
		auto imageReaderThread = std::make_unique<QThread>();

		m_imageReader->moveToThread(imageReaderThread.get());

		// Interconnect the reader's thread destruction chain.
		//
		QObject::connect(m_imageReader.get(), &OVRVisionImageReader::destroyed, imageReaderThread.get(), &QThread::quit); // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(imageReaderThread.get(), &QThread::finished, imageReaderThread.get(), &QThread::deleteLater); // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Interconnect the reader work startup.
		//
		QObject::connect(imageReaderThread.get(), &QThread::started, m_imageReader.get(), &OVRVisionImageReader::read); // once the thread starts, it will trigger the work of the reader

		// Interconnect the output of the reader.
		//
		QObject::connect(m_imageReader.get(), &OVRVisionImageReader::leftImageCaptured, this, static_cast<void (FirstPersonViewApp::*)(MatPtr)>(&FirstPersonViewApp::setLeftEyeImage));
		QObject::connect(m_imageReader.get(), &OVRVisionImageReader::rightImageCaptured, this, static_cast<void (FirstPersonViewApp::*)(MatPtr)>(&FirstPersonViewApp::setRightEyeImage));

		// Interconnect the reader destruction chain.
		//
		QObject::connect(this, &FirstPersonViewApp::destroyed, m_imageReader.get(), &OVRVisionImageReader::deleteLater);


		// Start reading.
		//
		imageReaderThread->start();

		// Release the smartpointer's ownership of the reader's thread.
		//
		imageReaderThread.release();

		// Release the smartpointer's ownership of the reader.
		//
		m_imageReader.release();
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
