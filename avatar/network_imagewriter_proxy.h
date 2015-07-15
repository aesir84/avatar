#pragma once

#include "image_writer.h"

namespace avatar
{

	class NetworkImageWriterProxy : public ImageWriter
	{

		friend class NetworkImageWriter;

		Q_OBJECT

	public slots:

		virtual void startWriting() override;
		virtual void writeImage(ImagePtr image) override;

	signals:

		void imageAvailable(ImagePtr image);

	private:

		NetworkImageWriterProxy(QHostAddress const & hostAddress, quint16 hostPort);

	private:

		QHostAddress m_hostAddress;
		quint16 m_hostPort;

		/// \brief A link to the writer object
		///
		/// The writer is stored as a raw pointer on purpose.
		/// On the one hand, the link to that object is required
		/// in order to query its state. On the other hand though,
		/// proxy's destructor must not try to destroy this object,
		/// because it resides in another thread. The writer's object
		/// destruction is established through the use of signals.
		/// \sa writeImage()
		/// \sa startWriting()
		///
		NetworkImageWriter * m_networkImageWriter;

	};

}
