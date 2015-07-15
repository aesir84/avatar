#include "stdafx.h"

#include "network_imagewriter.h"

#include "network_datasender.h"
#include "network_imageheader.h"
#include "network_imagesettings.h"
#include "network_imagewriter_proxy.h"

namespace avatar
{

	std::unique_ptr<ImageWriter> NetworkImageWriter::create(QHostAddress const & hostAddress, quint16 hostPort)
	{
		std::unique_ptr<NetworkImageWriterProxy> proxy(new NetworkImageWriterProxy(hostAddress, hostPort));
		return std::move(proxy);
	}

	NetworkImageWriter::NetworkImageWriter(QHostAddress const & hostAddress, quint16 hostPort)
		: m_hostAddress(hostAddress)
		, m_hostPort(hostPort)
		, m_dataSenderState(DataSenderState::Idle)
		, m_busy(false)
	{}

	bool NetworkImageWriter::isBusy() const
	{
		return m_busy;
	}

	void NetworkImageWriter::startWriting()
	{

		//
		// Setup the data sender in its dedicated thread.
		//

		auto dataSender = std::make_unique<NetworkDataSender>(m_hostAddress, m_hostPort);
		auto dataSenderThread = std::make_unique<QThread>();

		dataSender->moveToThread(dataSenderThread.get());

		// Interconnect the data sender's thread destruction chain.
		//
		QObject::connect(dataSender.get(), &NetworkDataSender::destroyed, dataSenderThread.get(), &QThread::quit);   // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(dataSenderThread.get(), &QThread::finished, dataSenderThread.get(), &QThread::deleteLater); // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Interconnect the working chain.
		//
		QObject::connect(dataSenderThread.get(), &QThread::started, dataSender.get(), &NetworkDataSender::connect);  // once the thread starts, it will trigger the worker to establish network connection
		QObject::connect(this, &NetworkImageWriter::dataReady, dataSender.get(), &NetworkDataSender::sendData);      // deliver the to be sent to the sender
		QObject::connect(dataSender.get(), &NetworkDataSender::dataSent, this, &NetworkImageWriter::processDataSent);// tell the writer that the data has been sent

		// Interconnect the data sender's destruction chain.
		//
		QObject::connect(this, &NetworkImageWriter::destroyed, dataSender.get(), &NetworkDataSender::deleteLater);

		// Start working.
		//
		dataSenderThread->start();

		// Release the smartpointer's ownership of the thread.
		//
		dataSenderThread.release();

		// Release the smartpointer's ownership of the data sender.
		//
		m_dataSender = dataSender.release();
	}

	void NetworkImageWriter::writeImage(ImagePtr image)
	{
		Q_ASSERT(!m_busy && m_dataSenderState == DataSenderState::Idle);

		m_dataSenderState = DataSenderState::Initializing;
		executeDataSender(std::make_tuple(image, false));
	}

	void NetworkImageWriter::executeDataSender(DataSenderInput const & input)
	{

		//
		// Run the sender state machine.
		//

		switch (m_dataSenderState)
		{

			case DataSenderState::Initializing:
			{
				Q_ASSERT(std::get<DataSenderInput_ImageToSend>(input) != nullptr);

				m_busy = true;

				m_serializedImage = serializeImage(*std::get<DataSenderInput_ImageToSend>(input));
				m_serializedImageHeader = serializeImageHeader(m_serializedImage);

				// Set the state that is about to be entered - the next state.
				//
				m_dataSenderState = DataSenderState::SendingImageHeader;
				
				emit dataReady(m_serializedImageHeader);
			}
			break;

			case DataSenderState::SendingImageHeader:
			{
				Q_ASSERT(std::get<DataSenderInput_DataSent>(input));

				// Set the state that is about to be entered - the next state.
				//
				m_dataSenderState = DataSenderState::SendingImage;

				emit dataReady(m_serializedImage);
			}
			break;

			case DataSenderState::SendingImage:
			{
				Q_ASSERT(std::get<DataSenderInput_DataSent>(input));

				m_dataSenderState = DataSenderState::Idle;

				m_busy = false;
			}
			break;

		}
	}

	QByteArray NetworkImageWriter::serializeImage(QImage const & image) const
	{
		QByteArray serializedImage;

		QBuffer serializationBuffer(&serializedImage);
		serializationBuffer.open(QIODevice::WriteOnly);

		image.save(&serializationBuffer, NetworkImageSettings::getImageFormat(), NetworkImageSettings::getImageQuality()); // this may be a long operation
		QByteArray const compressedSerializedImage = qCompress(serializedImage, 9);

		return compressedSerializedImage;
	}

	QByteArray NetworkImageWriter::serializeImageHeader(QByteArray const & serializedImage) const
	{
		NetworkImageHeader imageHeader(serializedImage.size());

		QByteArray serializedImageHeader;
		QDataStream serializationDataStream(&serializedImageHeader, QIODevice::WriteOnly);

		serializationDataStream << imageHeader;

		return serializedImageHeader;
	}

	void NetworkImageWriter::processDataSent()
	{
		executeDataSender(std::make_tuple(ImagePtr(), true));
	}

}
