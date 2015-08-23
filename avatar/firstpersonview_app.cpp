#include "stdafx.h"

#include "firstpersonview_app.h"

#include "exception.h"
#include "image_system.h"
#include "serial_datasender.h"
#include "webcam_imageprocessor.h"
#include "webcam_imagereader.h"

namespace avatar
{

	FirstPersonViewApp::FirstPersonViewApp()
		: m_stdOutput(stdout)
		, m_previousServoYaw(0)
		, m_previousServoPitch(0)
		, m_serialDataSender(nullptr)
	{ }

	void FirstPersonViewApp::initializeApp()
	{
		VideoStreamMonoApp::initializeApp();

		QObject::connect(this, &FirstPersonViewApp::orientationChanged, this, &FirstPersonViewApp::rotateCamera);


		//
		// Search for arduino serial port.
		//

		QList<QSerialPortInfo> const ports = QSerialPortInfo::availablePorts();
		QList<QSerialPortInfo>::const_iterator arduinoPort = ports.cend();

		for (auto it = ports.cbegin(); it != ports.cend(); ++it)
		{
			QString const description = it->description();

			if (description.contains("arduino", Qt::CaseInsensitive))
			{
				arduinoPort = it;
				break;
			}
		}

		if (arduinoPort == ports.cend())
		{
			throw Exception("Arduino error", "the device port is not found");
		}
		
		//
		// Setup the serial data sender in its dedicated thread.
		//

		// Testing this value currently...
		//
		int const serialCommTimeout = 50;

		auto serialDataSender = std::make_unique<SerialDataSender>(*arduinoPort, QSerialPort::Baud115200, "Arduino>", serialCommTimeout);
		auto serialDataSenderThread = std::make_unique<QThread>();

		serialDataSender->moveToThread(serialDataSenderThread.get());

		// Interconnect the sender initialization.
		//
		QObject::connect(serialDataSenderThread.get(), &QThread::started, serialDataSender.get(), &SerialDataSender::initialize); // once the thread starts, it will trigger the work of the sender

		// Interconnect the sender's input working chain.
		//
		QObject::connect(this, &FirstPersonViewApp::serialDataReady, serialDataSender.get(), &SerialDataSender::sendData); // once there is new data to send, do it

		// Interconnect the sender's error signals.
		//
		QObject::connect(serialDataSender.get(), &SerialDataSender::initializationFailed, this, &FirstPersonViewApp::raiseException);
		QObject::connect(serialDataSender.get(), &SerialDataSender::communicationFailed, this, &FirstPersonViewApp::reportProblem);

		// Interconnect the sender's destruction chain.
		//
		QObject::connect(this, &FirstPersonViewApp::destroyed, serialDataSender.get(), &SerialDataSender::deleteLater); // once the current object is destroyed, it will tell the worker to be destroyed as well

		// Interconnect the sender's thread destruction chain.
		//
		QObject::connect(serialDataSender.get(), &SerialDataSender::destroyed, serialDataSenderThread.get(), &QThread::quit); // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(serialDataSenderThread.get(), &QThread::finished, serialDataSenderThread.get(), &QThread::deleteLater); // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Start the sender's thread.
		//
		serialDataSenderThread->start();

		// Release the smartpointer's ownership of the sender's thread.
		//
		serialDataSenderThread.release();

		// Release the smartpointer's ownership of the sender.
		//
		m_serialDataSender = serialDataSender.release();


		//
		// Setup a webcam.
		//

		auto const cameras = QCameraInfo::availableCameras();

		if (cameras.empty())
		{
			throw Exception("Webcam error", "no available devices found");
		}

		m_imageSystem = ImageSystem::create(std::make_unique<WebcamImageReader>(cameras[0]),
			                                std::make_unique<WebcamImageProcessor>());
		QObject::connect(m_imageSystem.get(), &ImageSystem::imageReady, this, &FirstPersonViewApp::setEyeImage);
		QObject::connect(m_imageSystem.get(), &ImageSystem::errorOccured, this, &FirstPersonViewApp::raiseVideoStreamException);

		m_imageSystem->run();
	}

	void FirstPersonViewApp::rotateCamera(float yaw, float pitch, float roll)
	{
		// Currently the motor installation only supports yaw and pitch rotation.
		//
		Q_UNUSED(roll);

		// Preventing too much spam sent to the sender.
		//
		if (m_serialDataSender->isBusy()) // method isBusy() supports direct interthread invokation
		{
			return;
		}


		//
		// Communicate servo angles, derived from the received euler angles, to the Arduino.
		//

		int const servoYaw = getServoAngle(yaw);
		int const servoPitch = getServoAngle(pitch);

		// Omit the communication if both angles are exactly the same as their previous variants.
		//
		if (servoYaw == m_previousServoYaw && servoPitch == m_previousServoPitch)
		{
			return;
		}

		QString const servoTextCmd = createServoTextCommand(yaw, pitch);
		QByteArray dataToSend(servoTextCmd.toUtf8());

		Q_EMIT serialDataReady(dataToSend);

		// Save the most recent angles,
		// so that later, if these angles don't change,
		// it is possible to omit unnecessary communication.
		//
		m_previousServoYaw = servoYaw;
		m_previousServoPitch = servoPitch;
	}

	void FirstPersonViewApp::raiseException(QString const & exceptionDescription)
	{
		throw Exception("Arduino error", exceptionDescription);
	}

	void FirstPersonViewApp::reportProblem(QString const & problemDescription)
	{
		m_stdOutput << problemDescription << endl;
	}

	int FirstPersonViewApp::getServoAngle(float eulerAngle) const
	{
		float eulerAngleInDegrees = qRadiansToDegrees(eulerAngle);

		float const maxEulerAngle = 75.0f;
		float const minEulerAngle = -75.0f;

		if (eulerAngleInDegrees > maxEulerAngle)
		{
			eulerAngleInDegrees = maxEulerAngle;
		}
		else if (eulerAngleInDegrees < minEulerAngle)
		{
			eulerAngleInDegrees = minEulerAngle;
		}

		// Difference between Oculus and Servo angle representation.
		//
		//  Oculus:
		//
		//  180         90         0         -90       -180
		//   |----------|----------|----------|----------|
		//
		//              Servo:
		//
		//              0         90         180
		//              |----------|----------|
		//
		int servoAngle = 0;

		if (eulerAngleInDegrees < 0.0f)
		{
			servoAngle = static_cast<int>(qAbs(eulerAngleInDegrees) + 90.0f);
		}
		else
		{
			servoAngle = static_cast<int>(90.0f - eulerAngleInDegrees);
		}

		return servoAngle;
	}

	QString FirstPersonViewApp::createServoTextCommand(int servoYaw, int servoPitch) const
	{
		int const digitsCount = 3;
		int digitsRadix = 10;
		QChar const digitsPadding('0');

		return
			QString("Y") // 'Y' for yaw
			+
			QString("%1").arg(servoYaw, digitsCount, digitsRadix, digitsPadding) // yaw value
			+
			QString('P') // 'P' for pitch
			+
			QString("%1").arg(servoPitch, digitsCount, digitsRadix, digitsPadding); // pitch value
	}

}
