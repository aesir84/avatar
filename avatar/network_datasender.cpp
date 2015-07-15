#include "stdafx.h"

#include "network_datasender.h"

namespace avatar
{

	NetworkDataSender::NetworkDataSender(QHostAddress const & hostAddress, quint16 hostPort)
		: m_hostAddress(hostAddress)
		, m_hostPort(hostPort)
		, m_dataBytesToWriteIter(0)
	{ }

	void NetworkDataSender::connect()
	{
		m_socket = std::make_unique<QUdpSocket>();
		QObject::connect(m_socket.get(), static_cast<void (QUdpSocket::*)(QAbstractSocket::SocketError)>(&QUdpSocket::error), this, &NetworkDataSender::reportSocketError);

		m_socket->connectToHost(m_hostAddress, m_hostPort);

		m_nextDataBytesWritingTimer = std::make_unique<QTimer>();
		QObject::connect(m_nextDataBytesWritingTimer.get(), &QTimer::timeout, this, &NetworkDataSender::writeNextDataBytes);

		m_nextDataBytesWritingTimer->setInterval(1);
	}

	void NetworkDataSender::sendData(QByteArray const & data)
	{
		initializeWriting(data);
		writeFirstBytes();
	}

	void NetworkDataSender::initializeWriting(QByteArray const & data)
	{
		m_sendingAccess.lock();
		initializeDataBytesToWrite(data);
	}

	void NetworkDataSender::initializeDataBytesToWrite(QByteArray const & data)
	{
		Q_ASSERT(!data.isEmpty());

		m_dataBytesToWrite.clear();

		//
		// Reserve enough vector capacity to avoid reallocations.
		//

		int const elemCountToReserve = data.size() / DefaultNumberOfBytesToWritePerIteration;
		m_dataBytesToWrite.reserve(elemCountToReserve); // if elemCountToReserve is zero, than this is just a nop


		//
		// Fill the vector with BytesToWrite pairs.
		//

		int dataSizeAlreadyProcessed = 0;
		do
		{
			char const * bytesPtr = data.constData() + dataSizeAlreadyProcessed;

			int const dataSizeLeftToProcess = data.size() - dataSizeAlreadyProcessed;
			int const bytesSize = (dataSizeLeftToProcess >= DefaultNumberOfBytesToWritePerIteration) ? DefaultNumberOfBytesToWritePerIteration : dataSizeLeftToProcess;

			m_dataBytesToWrite.push_back(std::make_pair(bytesPtr, bytesSize));

			dataSizeAlreadyProcessed += bytesSize;

		} while (dataSizeAlreadyProcessed != data.size());


		//
		// Make a copy of the data.
		//

		m_data = data; // QByteArray is implicitly shared, so it is cheap to copy
	}

	void NetworkDataSender::writeFirstBytes()
	{
		m_dataBytesToWriteIter = 0;
		writeBytes();

		m_nextDataBytesWritingTimer->start();
	}

	void NetworkDataSender::writeNextBytes()
	{
		Q_ASSERT(isNextWriteValid());

		++m_dataBytesToWriteIter;
		writeBytes();
	}

	void NetworkDataSender::writeBytes()
	{
		m_socket->write(std::get<BytesToWrite_Ptr>(m_dataBytesToWrite[m_dataBytesToWriteIter]),
			            std::get<BytesToWrite_Size>(m_dataBytesToWrite[m_dataBytesToWriteIter]));
	}

	bool NetworkDataSender::isNextWriteValid() const
	{
		return m_dataBytesToWriteIter + 1 != m_dataBytesToWrite.size();
	}

	void NetworkDataSender::finalizeWriting()
	{
		m_nextDataBytesWritingTimer->stop();

		emit dataSent();

		m_sendingAccess.unlock();
	}

	void NetworkDataSender::writeNextDataBytes()
	{
		if (isNextWriteValid())
		{
			writeNextBytes();
			return;
		}

		finalizeWriting();
	}

	void NetworkDataSender::reportSocketError(QAbstractSocket::SocketError socketError)
	{
		Q_UNUSED(socketError);

		qDebug() << m_socket->errorString();
		emit senderBroken(m_socket->errorString());
	}

}
