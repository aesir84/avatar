#pragma once

#include "image_typedef.h"

namespace avatar
{

	class ImageSystemModule : public QObject
	{

		Q_OBJECT

	public:

		enum class ModuleOperation
		{
			ImageReading,
			ImageProcessing,
			ImageWriting
		};

	public:

		virtual ModuleOperation getOperationType() const = 0;

	Q_SIGNALS:

		void imageProcessed(ImagePtr image);
		void moduleBroken(QString description);

	private:

		friend class ImageSystem;

		virtual void initialize() = 0;
		virtual void processImage(ImagePtr image) = 0;

	};

}
