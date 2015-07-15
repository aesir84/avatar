#pragma once

#include "image_typedef.h"

namespace avatar
{

	/// \brief An abstract class which represents an image reader
	///
	/// The image reader acts as a source of images.
	/// It can be a webcam, a video player or even a network socket supplying image data.
	///
	/// The class is designed so, that objects of this class can and should be run in a separate thread.
	/// That is, connecting QThread::started() signal to ImageReader::startReading() slot to launch the thread.
	///
	class ImageReader : public QObject
	{

		Q_OBJECT

	public:

		virtual ~ImageReader();

	public slots:

		virtual void startReading() = 0;

	signals:

		void imageRead(ImagePtr image);
		void readerBroken(QString description);

	};

}
