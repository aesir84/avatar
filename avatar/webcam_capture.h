#pragma once

#include "imagesystem_typedefs.h"

namespace avatar
{

	/// \brief Webcam image capture
	///
	/// The class deals with capturing images - frames - from a webcam.
	/// It does so by publicly subclassing an abstract class QAbstractVideoSurface.
	/// The intention of this inheritance is more like is-implemented-in-terms-of,
	/// though private inheritance will not work in this case, because signals and slots
	/// mechanism needs access to QObject interface to work. Still both pure virtual
	/// methods - present() and supportedPixelFormats - are made private, because this
	/// class should not expose this interface.
	///
	class WebcamCapture : public QAbstractVideoSurface
	{

		Q_OBJECT

	public:

		WebcamCapture();

	signals:

		void imageCaptured(ImagePtr image);

	private:

		virtual bool present(QVideoFrame const & frame) override;
		virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override;

	};

}
