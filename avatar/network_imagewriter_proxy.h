#pragma once

#include "imagesystem_module.h"

namespace avatar
{

	class NetworkImageWriterProxy : public ImageSystemModule
	{

		Q_OBJECT

	public:

		virtual ModuleOperation getOperationType() const override;

	private:

		/// \brief A friend class to construct an object of this class
		///
		/// The proxy is only meant to work in pair with an object of class NetworkImageWriter.
		/// Hence, only such objects are allowed to create the proxies.
		///
		friend class NetworkImageWriter;

		NetworkImageWriterProxy(QHostAddress const & hostAddress, quint16 hostPort);

	private:

		virtual void initialize() override;
		virtual void processImage(ImagePtr image) override;

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
