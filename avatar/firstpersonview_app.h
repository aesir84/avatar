#pragma once

#include "videostream_monoapp.h"

namespace avatar
{

	class ImageSystem;
	class SerialDataSender;

	class ArduinoApp : public VideoStreamMonoApp
	{

		Q_OBJECT

	public:

		ArduinoApp();

	protected:

		virtual void initializeApp() override final;

	signals:

		/// \brief A signal to communicate with a serial data sender
		///
		/// Due to the fact, that the sender runs in a dedicated thread,
		/// it is crucial to communicate with it using a signal/slot
		/// mechanism.
		///
		void serialDataReady(QByteArray const & data);

	private:

		void rotateCamera(float yaw, float pitch, float roll);
		void raiseException(QString const & exceptionDescription);
		void reportProblem(QString const & problemDescription);

	private:

		int getServoAngle(float eulerAngle) const;
		QString createServoTextCommand(int servoYaw, int servoPitch) const;

	private:

		QTextStream m_stdOutput;

		int m_previousServoYaw;
		int m_previousServoPitch;

		/// \brief A link to serial data sender
		///
		/// The sender is not owned by this class,
		/// thus it is a raw pointer.
		/// This link is for convenience to call a pair of methods
		/// from the sender, which do not require signal/slot mechanism.
		///
		SerialDataSender * m_serialDataSender;

		std::unique_ptr<ImageSystem> m_imageSystem;

	};

}
