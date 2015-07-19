#pragma once

#include "image_typedef.h"

namespace avatar
{

	/// \brief An abstract class which represents an image reader
	///
	/// The image reader acts as a source of images.
	/// It can be a webcam, a video player or even a network socket supplying image data.
	///
	/// The class is designed so, that only ImageSystem class is able to setup the class's initialization.
	/// Such delayed initialization is suitable for starting the reader in its own dedicated thread.
	///
	class ImageReader : public QObject
	{

		Q_OBJECT

	public:

		virtual ~ImageReader();

	Q_SIGNALS:

		void imageRead(ImagePtr image);
		void readerBroken(QString description);

	private:

		friend class ImageSystem;

		virtual void initialize() = 0;

	};

}
