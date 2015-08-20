#include "stdafx.h"

#include "serial_datasender.h"

#include "scopeguard.h"

namespace avatar
{

	SerialDataSender::SerialDataSender(QSerialPortInfo const & portInfo, qint32 baudRate, QByteArray const & prompt, int timeout)
		: m_portInfo(portInfo)
		, m_baudRate(baudRate)
		, m_prompt(prompt)
		, m_halfTimeout(timeout / 2)
		, m_busy(false)
	{ }

	bool SerialDataSender::isBusy() const
	{
		return m_busy;
	}

	void SerialDataSender::initialize()
	{

		//
		// Setup the serial port.
		//

		m_port = std::make_unique<QSerialPort>(m_portInfo);
		m_port->setBaudRate(m_baudRate);
		if (!m_port->open(QIODevice::ReadWrite))
		{
			Q_EMIT initializationFailed(QString("Serial data sender :: failed to open %1 :: %2").arg(m_port->portName()).arg(m_port->errorString()));
			return;
		}
	}

	void SerialDataSender::sendData(QByteArray const & data)
	{
		// The object of this class must have been already initialized.
		//
		Q_ASSERT(isInitialized());

		m_busy = true;
		auto busyGuard = guardScope([&]{ m_busy = false; });

		m_port->write(data);

		if (!m_port->waitForBytesWritten(m_halfTimeout))
		{
			if (m_port->error() == QSerialPort::TimeoutError)
			{
				m_port->clear();
			}
			Q_EMIT communicationFailed(QString("Serial data sender :: failed to write the data :: %1").arg(m_port->errorString()));
			return;
		}

		if (!m_port->waitForReadyRead(m_halfTimeout))
		{
			if (m_port->error() == QSerialPort::TimeoutError)
			{
				m_port->clear();
			}
			Q_EMIT communicationFailed(QString("Serial data sender :: failed to read the response :: %1").arg(m_port->errorString()));
			return;
		}

		QByteArray response = m_port->readAll();

		if (!response.contains(m_prompt))
		{
			int const interCharacterTimeout = 10;

			while (m_port->waitForReadyRead(interCharacterTimeout))
			{
				response += m_port->readAll();
			}

			if (!response.contains(m_prompt))
			{
				if (m_port->error() == QSerialPort::TimeoutError)
				{
					m_port->clear();
				}
				Q_EMIT communicationFailed(QString("Serial data sender :: failed to read the prompt :: %1").arg(m_port->errorString()));
				return;
			}
		}

		Q_EMIT dataSent();
	}

	bool SerialDataSender::isInitialized() const
	{
		return m_port->isOpen();
	}

}
