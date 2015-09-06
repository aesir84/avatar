#pragma once

#include "firstpersonview_cli.h"

namespace avatar
{

	class SerialDataSender;

	class FirstPersonViewSerialCli : public FirstPersonViewCli
	{

		Q_OBJECT

	public:

		static std::unique_ptr<FirstPersonViewSerialCli> create(QSerialPortInfo const & serialPortInfo, qint32 serialCommBaudRate, QByteArray const & serialCommPrompt, int serialCommTimeout);

	signals:

		/// \brief A signal to communicate with a serial data sender
		///
		/// Due to the fact, that the sender runs in a dedicated thread,
		/// it is crucial to communicate with it using a signal/slot
		/// mechanism.
		///
		void serialDataReady(QByteArray const & data);

	private:

		FirstPersonViewSerialCli(QSerialPortInfo const & serialPortInfo, qint32 serialCommBaudRate, QByteArray const & serialCommPrompt, int serialCommTimeout);

	private:

		virtual void initializeImpl() override;
		virtual void sendCommandImpl(QByteArray const & command) override;

	private:

		/// \brief Serial port initialization data
		///
		/// Due to the fact that the class uses a delayed initialization,
		/// this data has to be remembered.
		///
		QSerialPortInfo m_serialPortInfo;
		qint32 m_serialCommBaudRate;
		QByteArray m_serialCommPrompt;
		int m_serialCommTimeout;

		/// \brief A link to the serial data sender
		///
		/// The sender has a method called isBusy(), which can be invoked directly,
		/// i.e. without the use of interthread signal/slot mechanism.
		/// Therefore, this link is used solely for this purpose.
		/// Note : this class does not own the sender, thus, a simple raw pointer
		/// will suffice in this case.
		///
		SerialDataSender * m_serialDataSender;

	};

}
