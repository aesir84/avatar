#pragma once

#include "imagesystem_module.h"

namespace avatar
{

	class NetworkImageReader : public ImageSystemModule
	{

		Q_OBJECT

	public:

		explicit NetworkImageReader(quint16 port);

	public:

		virtual ModuleOperation getOperationType() const override;

	private:

		enum class ReaderState
		{
			Idle,
			WaitingForNetworkImageHeader,
			WaitingForNetworkImage
		};

	private:

		virtual void initialize() override;
		virtual void processImage(ImagePtr image) override;

	private:

		void readDatagrams();
		void readImageFromDatagram(QByteArray const & datagram);

	private:

		std::unique_ptr<QUdpSocket> m_socket;

		quint16 m_port;
		ReaderState m_readerState;
		qint64 m_expectedNetworkImageSize;
		QByteArray m_receivedNetworkImage;

	};

}
