#pragma once

namespace avatar
{

	/// \brief A class to send byte data over a network
	///
	/// The class uses UDP protocol for network communication.
	/// Objects of this class are meant to be used in their dedicated threads.
	/// Failure to follow this pattern may lead to undefined behavior, such as deadlocks.
	/// In order to fully initialize an object of this class, one needs to connect
	/// QThread::started() signal to NetworkDataSender::connect() slot.
	/// The class uses a mutex to establish an exclusive access to sending features.
	/// This exclusive access lasts from execution of NetworkDataSender::sendData() slot
	/// till emission of NetworkDataSender::dataSent() signal.
	///
	class NetworkDataSender : public QObject
	{

		Q_OBJECT

	public:

		NetworkDataSender(QHostAddress const & hostAddress, quint16 hostPort);

	public slots:

		void connect();
		void sendData(QByteArray const & data);

	signals:

		void dataSent();
		void senderBroken(QString description);

	private:

		/// \brief Information about a chunk of bytes to be written to a socket
		///
		/// This type relies on the fact, that the data it points to remains valid
		/// as long as this type object is used. Otherwise there will be an undefined behavior.
		/// So basically it is just a helper type for QByteArray, because the latter one allows
		/// to reference its internal buffer through pointers.
		///
		using BytesToWrite = std::pair<
			char const *, // bytes pointer
			int           // bytes size
		>;
		enum BytesToWriteFields { BytesToWrite_Ptr, BytesToWrite_Size };

		using DataBytesToWrite = std::vector<BytesToWrite>;

	private:

		void initializeWriting(QByteArray const & data);
		void initializeDataBytesToWrite(QByteArray const & data);

		void writeFirstBytes();
		void writeNextBytes();
		void writeBytes();

		bool isNextWriteValid() const;

		void finalizeWriting();

	private slots:

		void writeNextDataBytes();
		void reportSocketError(QAbstractSocket::SocketError socketError);

	private:

		QHostAddress m_hostAddress;
		quint16 m_hostPort;

		std::unique_ptr<QUdpSocket> m_socket;
		std::unique_ptr<QTimer> m_nextDataBytesWritingTimer;

		QByteArray m_data;
		DataBytesToWrite m_dataBytesToWrite;

		size_t m_dataBytesToWriteIter;

		/// \brief Default size of byte chunks which are written to the socket.
		///
		/// The size is less than a default MTU for Ethernet (1500 bytes) to avoid datagram fragmentation.
		/// The size is a power of two to allow fast division in getBytesToSendFromSerializedData() method.
		/// \sa getBytesToSendFromSerializedData()
		///
		static int const DefaultNumberOfBytesToWritePerIteration = 1024;

		std::mutex m_sendingAccess;

	};

}
