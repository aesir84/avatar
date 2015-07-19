#pragma once

#include "image_writer.h"

namespace avatar
{

	class NetworkDataSender;

	class NetworkImageWriter : public ImageWriter
	{

		Q_OBJECT

	public:

		static std::unique_ptr<ImageWriter> create(QHostAddress const & hostAddress, quint16 hostPort);

		bool isBusy() const;

	Q_SIGNALS:

		void dataReady(QByteArray const & data);

	private:

		enum class DataSenderState
		{
			Idle,
			Initializing,
			SendingImageHeader,
			SendingImage
		};

		using DataSenderInput = std::tuple<
			ImagePtr, // image to send
			bool      // just in case variable that signalises that the data was sent successfully
		>;
		enum DataSenderInputFields { DataSenderInput_ImageToSend, DataSenderInput_DataSent };

	private:

		friend class NetworkImageWriterProxy;

		NetworkImageWriter(QHostAddress const & hostAddress, quint16 hostPort);

	private:

		void executeDataSender(DataSenderInput const & input);

		QByteArray serializeImage(QImage const & image) const;
		QByteArray serializeImageHeader(QByteArray const & serializedImage) const;

		void processDataSent();

	private:

		virtual void initialize() override;
		virtual void writeImage(ImagePtr image) override;

	private:

		QHostAddress m_hostAddress;
		quint16 m_hostPort;

		NetworkDataSender * m_dataSender;
		DataSenderState m_dataSenderState;

		std::atomic<bool> m_busy;

		QByteArray m_serializedImage;
		QByteArray m_serializedImageHeader;

	};

}
