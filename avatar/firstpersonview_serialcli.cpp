#include "stdafx.h"

#include "firstpersonview_serialcli.h"

#include "serial_datasender.h"

namespace avatar
{

	std::unique_ptr<FirstPersonViewSerialCli> FirstPersonViewSerialCli::create(QSerialPortInfo const & serialPortInfo, qint32 serialCommBaudRate, QByteArray const & serialCommPrompt, int serialCommTimeout)
	{
		return std::unique_ptr<FirstPersonViewSerialCli>(new FirstPersonViewSerialCli(serialPortInfo, serialCommBaudRate, serialCommPrompt, serialCommTimeout));
	}

	FirstPersonViewSerialCli::FirstPersonViewSerialCli(QSerialPortInfo const & serialPortInfo, qint32 serialCommBaudRate, QByteArray const & serialCommPrompt, int serialCommTimeout)
		: m_serialPortInfo(serialPortInfo)
		, m_serialCommBaudRate(serialCommBaudRate)
		, m_serialCommPrompt(serialCommPrompt)
		, m_serialCommTimeout(serialCommTimeout)
	{ }

	void FirstPersonViewSerialCli::initializeImpl()
	{

		//
		// Setup the serial data sender in its dedicated thread.
		//

		auto serialDataSender = std::make_unique<SerialDataSender>(m_serialPortInfo, m_serialCommBaudRate, m_serialCommPrompt, m_serialCommTimeout);
		auto serialDataSenderThread = std::make_unique<QThread>();

		serialDataSender->moveToThread(serialDataSenderThread.get());

		// Interconnect the sender initialization.
		//
		QObject::connect(serialDataSenderThread.get(), &QThread::started, serialDataSender.get(), &SerialDataSender::initialize); // once the thread starts, it will trigger the work of the sender

		// Interconnect the sender's input working chain.
		//
		QObject::connect(this, &FirstPersonViewSerialCli::serialDataReady, serialDataSender.get(), &SerialDataSender::sendData); // once there is new data to send, do it

		// Interconnect the sender's error signals.
		//
		QObject::connect(serialDataSender.get(), &SerialDataSender::initializationFailed, this, &FirstPersonViewSerialCli::error);
		QObject::connect(serialDataSender.get(), &SerialDataSender::communicationFailed, this, &FirstPersonViewSerialCli::warning);

		// Interconnect the sender's destruction chain.
		//
		QObject::connect(this, &FirstPersonViewSerialCli::destroyed, serialDataSender.get(), &SerialDataSender::deleteLater); // once the current object is destroyed, it will tell the worker to be destroyed as well

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
	}

	void FirstPersonViewSerialCli::sendCommandImpl(QByteArray const & command)
	{
		// Preventing too much spam sent to the sender.
		//
		if (m_serialDataSender->isBusy()) // method SerialDataSender::isBusy() supports direct interthread invocation
		{
			return;
		}

		Q_EMIT serialDataReady(command);
	}

}
