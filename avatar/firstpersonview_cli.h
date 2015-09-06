#pragma once

#include "fpv_anglecalc.h"

namespace avatar
{

	class FirstPersonViewCli : public QObject
	{

		Q_OBJECT

	public:

		FirstPersonViewCli();

		/// \brief Create a text command to rotate the FPV
		///
		/// The method takes euler angles in radians, converts these angles
		/// to values, which suit FPV rotation, and outputs a corresponding
		/// text command, which is to be sent to command line interpreter.
		///
		template <typename FpvAxisAngleCalculator>
		static QByteArray createCommand(FpvAxisAngleCalculator calculator, float yaw, float pitch, float roll)
		{
			Q_UNUSED(roll);

			int const fpvYaw = calculator(fpv_utils::TwoAxisFpvAngleCalculator::Axis::Y, yaw);
			int const fpvPitch = calculator(fpv_utils::TwoAxisFpvAngleCalculator::Axis::X, pitch);

			int const digitsCount = 3;
			int digitsRadix = 10;
			QChar const digitsPadding('0');

			QString const command =
				QString("Y") // 'Y' for yaw
				+
				QString("%1").arg(fpvYaw, digitsCount, digitsRadix, digitsPadding) // yaw value
				+
				QString('P') // 'P' for pitch
				+
				QString("%1").arg(fpvPitch, digitsCount, digitsRadix, digitsPadding); // pitch value

			return command.toUtf8();
		}

	public:

		/// \brief A method to support a delayed initialization
		///
		/// The method uses a non-virtual interface idiom.
		/// Thus, the method defines the executive part of the method.
		/// The engineering part must be defined by the derived classes
		/// by overriding pure virtual initializeImpl() method.
		/// Such a structure is needed to prevent additional initializations.
		/// If such an extra initialization is invoked,
		/// then a warning() signal is emitted.
		/// \sa initializeImpl()
		///
		void initialize();

		/// \brief A method to send a text command to the FPV
		///
		/// The method uses a non-virtual interface idiom.
		/// Thus, the method defines the executive part of the method.
		/// The engineering part must be defined by the derived classes
		/// by overriding pure virtual sendCommandImpl() method.
		/// Such a structure is needed to allow the command line interpreter
		/// to skip commands that are exactly the same as the last sent commands.
		/// Hence, avoiding too much stress on the communication line.
		/// \sa sendCommandImpl()
		///
		void sendCommand(QByteArray const & command);

	signals:

		void commandSent();

		void error(QString const & description);
		void warning(QString const & description);

	private:

		/// \brief A pure virtual method to perform the delayed initialization
		///
		/// This method is called from initialize() method as part of NVI idiom.
		/// This method is the place where derived classes must put their initialization logic.
		/// \sa initialize()
		///
		virtual void initializeImpl() = 0;

		/// \brief A pure virtual method to send a command to the FPV
		///
		/// This method is called from sendCommand() method as part of NVI idiom.
		/// This method is the place where derived classes must put the sending logic.
		/// \sa sendCommand()
		///
		virtual void sendCommandImpl(QByteArray const & command) = 0;

	private:

		bool m_initialized;
		QByteArray m_lastSentCommand;

	};

}
