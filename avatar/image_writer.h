#pragma once

#include "image_typedef.h"

namespace avatar
{

	class ImageWriter : public QObject
	{

		Q_OBJECT

	public:

		virtual ~ImageWriter();

	public slots:

		virtual void startWriting() = 0;
		virtual void writeImage(ImagePtr image) = 0;

	signals:

		void writerBroken(QString description);

	};

}
