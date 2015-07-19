#pragma once

#include "image_typedef.h"

namespace avatar
{

	/// \brief An interface class for image output variations
	///
	/// One can output an image to a network socket or to a hard drive, for example.
	/// In general, this can be a rather lengthy operation, so the derived classes
	/// should count with it and apply some ways to deal with this circumstance.
	/// Since the class is part of the image processing pipeline, its initialization
	/// is governed by ImageSystem class, which must have exclusive access to initialization
	/// facilities.
	///
	class ImageWriter : public QObject
	{

		Q_OBJECT

	public:

		virtual ~ImageWriter();

	Q_SIGNALS:

		void writerBroken(QString description);

	private:

		friend class ImageSystem;

		virtual void initialize() = 0;
		virtual void writeImage(ImagePtr image) = 0;

	};

}
