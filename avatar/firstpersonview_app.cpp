#include "stdafx.h"

#include "arduino_app.h"

#include "exception.h"
#include "image_system.h"
#include "serial_datasender.h"
#include "webcam_imageprocessor.h"
#include "webcam_imagereader.h"

namespace avatar
{

	ArduinoApp::ArduinoApp()
		: m_stdOutput(stdout)
		, m_previousServoAngle(0)
		, m_serialDataSender(nullptr)
	{ }

	void ArduinoApp::initializeApp()
	{
		VideoStreamMonoApp::initializeApp();

		QObject::connect(this, &ArduinoApp::orientationChanged, this, &ArduinoApp::rotateCamera);


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
		QObject::connect(this, &ArduinoApp::serialDataReady, serialDataSender.get(), &SerialDataSender::sendData); // once there is new data to send, do it

		// Interconnect the sender's error signals.
		//
		QObject::connect(serialDataSender.get(), &SerialDataSender::initializationFailed, this, &ArduinoApp::raiseException);
		QObject::connect(serialDataSender.get(), &SerialDataSender::communicationFailed, this, &ArduinoApp::reportProblem);

		// Interconnect the sender's destruction chain.
		//
		QObject::connect(this, &ArduinoApp::destroyed, serialDataSender.get(), &SerialDataSender::deleteLater); // once the current object is destroyed, it will tell the worker to be destroyed as well

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
		QObject::connect(m_imageSystem.get(), &ImageSystem::imageReady, this, &ArduinoApp::setEyeImage);
		QObject::connect(m_imageSystem.get(), &ImageSystem::errorOccured, this, &ArduinoApp::raiseVideoStreamException);

		m_imageSystem->run();
	}

	void ArduinoApp::rotateCamera(float yaw, float pitch, float roll)
	{
		// The motor can be given three degrees of freedom,
		// although currently only one DoF is utilized.
		//
		Q_UNUSED(pitch);
		Q_UNUSED(roll);


		//
		// Preventing too much spam sent to the sender.
		//

		if (m_serialDataSender->isBusy())
		{
			return;
		}


		//
		// Convert the received values to the ones that suit arduino servo motor.
		//

		float angle = qRadiansToDegrees(yaw);

		// Servo may go 'crazy' if it is given edge values.
		// Especially it concerns one side of the range,
		// where its current will be at maximum. Such situations should be avoided.
		// Therefore, it is best to put some safety margins on the edges.
		//
		bool const validAngle = angle < 85.0f && angle > -80.0f;

		if (!validAngle)
		{
			return;
		}

		// Difference between Oculus and Servo angle representation.
		// Here the angles in both cases are in degrees,
		// although the values initially coming from Oculus are in radians.
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
		if (angle < 0.0f)
		{
			angle = qAbs(angle) + 90.0f;
		}
		else
		{
			angle = 90.0f - angle;
		}


		//
		// Construct a text command to send over a serial communication link.
		//

		// Arduino servo library implementation does not support float type values.
		// So there is no point in sending them over a wire in such a way.
		//
		qint16 const servoAngle = static_cast<qint16>(angle);

		if (servoAngle == m_previousServoAngle)
		{
			return;
		}

		QString const servoCommand = QString("rotate ") + QString::number(servoAngle);
		QByteArray dataToSend(servoCommand.toUtf8());

		m_stdOutput << "Rotating to " << servoAngle << " degrees" << endl;

		Q_EMIT serialDataReady(dataToSend);
	}

	void ArduinoApp::raiseException(QString const & exceptionDescription)
	{
		throw Exception("Arduino error", exceptionDescription);
	}

	void ArduinoApp::reportProblem(QString const & problemDescription)
	{
		m_stdOutput << problemDescription << endl;
	}

}
