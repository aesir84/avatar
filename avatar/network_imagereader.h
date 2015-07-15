#pragma once

#include "image_reader.h"

namespace avatar
{

	class NetworkImageReader : public ImageReader
	{

		Q_OBJECT

	public:

		explicit NetworkImageReader(quint16 port);

	public slots:

		virtual void startReading() override;

	private slots:

		void readDatagrams();

	private:

		enum class ReaderState
		{
			Idle,
			WaitingForNetworkImageHeader,
			WaitingForNetworkImage
		};

	private:

		void readImageFromDatagram(QByteArray const & datagram);

	private:

		std::unique_ptr<QUdpSocket> m_socket;

		quint16 m_port;
		ReaderState m_readerState;
		qint64 m_expectedNetworkImageSize;
		QByteArray m_receivedNetworkImage;

	};

}
