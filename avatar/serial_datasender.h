#pragma once

namespace avatar
{

	/// \brief A class which represents the master in a serial communication
	///
	/// Due to the fact, that this class uses blocking API for serial communication,
	/// it is crucial, that objects of this class are used in a separate thread,
	/// so that the most important parts of the public interface are invoked through a signal/slot mechanism.
	/// Running the objects in the client's thread and invoking this interface
	/// directly using mere function calls can lead to the client's thread freezing.
	///
	/// In order not to over encumber the thread queue it is possible to 
	///
	/// Prior to using sendData() method, an object of this class must be first initialized
	/// by invoking its initialize() method as a slot connected to QThread::started() signal.
	///
	/// After sending the data the implementation waits for a response from the slave.
	/// The response must end with a prompt string. This prompt is passed to the constructor
	/// of this class.
	///
	class SerialDataSender : public QObject
	{

		Q_OBJECT

	public:

		SerialDataSender(QSerialPortInfo const & portInfo, qint32 baudRate, QByteArray const & prompt, int timeout);

	public:

		/// \brief A method to check if the sender is busy sending some data
		///
		/// The use case for this method is when the client does not want to spam
		/// an object of this class with too many sending requests. Thus, the client
		/// can omit some requests if the server is currently busy, hence, keeping the
		/// server's thread queue lightweight.
		/// This method can be called directly from another thread, as the only thing
		/// affected by this call is an atomic boolean member m_busy.
		///
		bool isBusy() const;

	public:

		void initialize();
		void sendData(QByteArray const & data);

	signals:

		void initializationFailed(QString const & failureDescription);
		void communicationFailed(QString const & failureDescription);
		void dataSent();

	private:

		bool isInitialized() const;

	private:

		QSerialPortInfo m_portInfo;
		qint32 m_baudRate;
		QByteArray m_prompt;
		int m_halfTimeout;

		std::unique_ptr<QSerialPort> m_port;

		std::atomic<bool> m_busy;

	};

}
