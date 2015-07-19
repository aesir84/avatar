#include "stdafx.h"

#include "network_imagewriter_proxy.h"

#include "network_imagewriter.h"

namespace avatar
{

	NetworkImageWriterProxy::NetworkImageWriterProxy(QHostAddress const & hostAddress, quint16 hostPort)
		: m_hostAddress(hostAddress)
		, m_hostPort(hostPort)
	{}

	void NetworkImageWriterProxy::initialize()
	{

		//
		// Setup the image writer in its dedicated thread.
		//

		std::unique_ptr<NetworkImageWriter> networkImageWriter(new NetworkImageWriter(m_hostAddress, m_hostPort));
		auto networkImageWriterThread = std::make_unique<QThread>();

		networkImageWriter->moveToThread(networkImageWriterThread.get());

		// Interconnect the writer's thread destruction chain.
		//
		QObject::connect(networkImageWriter.get(), &NetworkImageWriter::destroyed, networkImageWriterThread.get(), &QThread::quit);  // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(networkImageWriterThread.get(), &QThread::finished, networkImageWriterThread.get(), &QThread::deleteLater); // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Interconnect the working chain.
		//
		QObject::connect(networkImageWriterThread.get(), &QThread::started, networkImageWriter.get(), &NetworkImageWriter::initialize); // once the thread starts, it will trigger the work of the writer
		QObject::connect(this, &NetworkImageWriterProxy::imageAvailable, networkImageWriter.get(), &NetworkImageWriter::writeImage);    // once the proxy has an available image, this image can be written

		// Interconnect the writer's destruction chain.
		//
		QObject::connect(this, &NetworkImageWriterProxy::destroyed, networkImageWriter.get(), &NetworkImageWriter::deleteLater);

		// Start working.
		//
		networkImageWriterThread->start();

		// Release the smartpointer's ownership of the thread.
		//
		networkImageWriterThread.release();

		// Release the smartpointer's ownership of the writer.
		//
		m_networkImageWriter = networkImageWriter.release();
	}

	void NetworkImageWriterProxy::writeImage(ImagePtr image)
	{
		Q_ASSERT(m_networkImageWriter != nullptr);

		if (m_networkImageWriter->isBusy())
		{
			return;
		}

		emit imageAvailable(image);
	}

}
