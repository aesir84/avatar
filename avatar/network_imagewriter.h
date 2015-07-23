#pragma once

#include "imagesystem_module.h"

namespace avatar
{

	class NetworkDataSender;

	class NetworkImageWriter : public ImageSystemModule
	{

		Q_OBJECT

	public:

		static std::unique_ptr<ImageSystemModule> create(QHostAddress const & hostAddress, quint16 hostPort);

	public:

		bool isBusy() const;

	public:

		virtual ModuleOperation getOperationType() const override;

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

		/// \brief A friend class to construct an object of this class
		///
		/// Keeping in mind the fact, that NetworkImageWriter class is implicitly managed through a proxy,
		/// the proxy needs access to the private constructor of class NetworkImageWriter in order to implicitly create it.
		/// There is no point for the proxy to use the factory method NetworkImageWriter::create, because this method once
		/// again yields a proxy object.
		///
		friend class NetworkImageWriterProxy;

		NetworkImageWriter(QHostAddress const & hostAddress, quint16 hostPort);

	private:

		void executeDataSender(DataSenderInput const & input);

		QByteArray serializeImage(QImage const & image) const;
		QByteArray serializeImageHeader(QByteArray const & serializedImage) const;

		void processDataSent();

	private:

		virtual void initialize() override;
		virtual void processImage(ImagePtr image) override;

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
