#include "stdafx.h"

#include "network_imagereader.h"

#include "network_imageheader.h"
#include "network_imagesettings.h"

namespace avatar
{

	namespace
	{
		bool readImageHeaderFromDatagram(QByteArray const & datagram, NetworkImageHeader & imageHeader)
		{
			// Prepare the datagram so it can be read as a device.
			//
			QBuffer deserializationBuffer(const_cast<QByteArray*>(&datagram)); // although there will only be read access, constness still needs to be cast away because of QBuffer constructor
			deserializationBuffer.open(QIODevice::ReadOnly);

			// Read the data from the device data stream.
			//
			QDataStream datagramDataStream(&deserializationBuffer);
			datagramDataStream >> imageHeader;

			return datagramDataStream.status() == QDataStream::Ok;
		}
	}

	NetworkImageReader::NetworkImageReader(quint16 port)
		: m_port(port)
		, m_readerState(ReaderState::Idle)
		, m_expectedNetworkImageSize(0)
	{}

	void NetworkImageReader::initialize()
	{
		m_socket = std::make_unique<QUdpSocket>();

		m_socket->bind(QHostAddress::Any, m_port);
		QObject::connect(m_socket.get(), &QUdpSocket::readyRead, this, &NetworkImageReader::readDatagrams);

		m_readerState = ReaderState::WaitingForNetworkImageHeader;
	}

	void NetworkImageReader::readDatagrams()
	{
		while (m_socket->hasPendingDatagrams())
		{
			int const datagramSize = static_cast<int>(m_socket->pendingDatagramSize());

			if (datagramSize != 0)
			{
				QByteArray datagram;
				datagram.resize(datagramSize);

				m_socket->readDatagram(datagram.data(), datagram.size());

				readImageFromDatagram(datagram);
			}
		}
	}

	void NetworkImageReader::readImageFromDatagram(QByteArray const & datagram)
	{

		//
		// Run the state machine.
		//

		switch (m_readerState)
		{
			case ReaderState::WaitingForNetworkImageHeader:
			{

				//
				// Try to get the header of an incoming image.
				// If the datagram contains a valid header,
				// then save the header information and switch the state.
				// Otherwise do nothing, hence, discarding any non-header data.
				//

				NetworkImageHeader remoteImageHeader;

				if (readImageHeaderFromDatagram(datagram, remoteImageHeader))
				{
					m_expectedNetworkImageSize = remoteImageHeader.m_imageSize;

					// The image header has been received correctly - change the state.
					//
					m_readerState = ReaderState::WaitingForNetworkImage;
				}
			}
			break;

			case ReaderState::WaitingForNetworkImage:
			{

				//
				// Try to get the data of an incoming image.
				// The datagrams handled by this state should not contain image headers.
				// If a datagram contains an image header, then some datagrams with data obviously got lost.
				// In this case, all the image data received so far is discarded, the state remains the same,
				// and the process of image data receiving starts anew.
				//

				NetworkImageHeader remoteImageHeader;

				if (!readImageHeaderFromDatagram(datagram, remoteImageHeader))
				{
					m_receivedNetworkImage.append(datagram);

					qint64 const receivedNetworkImageSize = m_receivedNetworkImage.size();

					if (receivedNetworkImageSize >= m_expectedNetworkImageSize)
					{
						QByteArray const uncompressedNetworkImage = qUncompress(m_receivedNetworkImage);
						auto image = std::make_shared<QImage>(QImage::fromData(uncompressedNetworkImage, NetworkImageSettings::getImageFormat()));
						emit imageRead(image);

						m_receivedNetworkImage.clear();

						// The whole image has been received correctly and processed - change the state.
						//
						m_readerState = ReaderState::WaitingForNetworkImageHeader;
					}
				}
				else
				{
					m_receivedNetworkImage.clear();
					m_expectedNetworkImageSize = remoteImageHeader.m_imageSize;
				}
			}
			break;

			default:
			{
				Q_ASSERT(false);
			}
		}
	}

}
