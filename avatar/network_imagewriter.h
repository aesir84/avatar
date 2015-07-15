#pragma once

#include "image_writer.h"

namespace avatar
{

	class NetworkDataSender;

	class NetworkImageWriter : public ImageWriter
	{

		friend class NetworkImageWriterProxy;

		Q_OBJECT

	public:

		static std::unique_ptr<ImageWriter> create(QHostAddress const & hostAddress, quint16 hostPort);

		bool isBusy() const;

	public slots:

		virtual void startWriting() override;
		virtual void writeImage(ImagePtr image) override;

	signals:

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

		NetworkImageWriter(QHostAddress const & hostAddress, quint16 hostPort);

		void executeDataSender(DataSenderInput const & input);

		QByteArray serializeImage(QImage const & image) const;
		QByteArray serializeImageHeader(QByteArray const & serializedImage) const;

	private slots:

		void processDataSent();

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
