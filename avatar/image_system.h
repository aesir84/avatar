#pragma once

#include "imagesystem_typedefs.h"

namespace avatar
{

	class ImageSystemModule;

	/// \brief A system that manages the image processing pipeline
	///
	/// The class represents a system, that binds together all the modules
	/// of the image processing pipeline, like reading, processing and writing.
	/// The class has an exclusive right to setup the modules, i.e. to connect the signals and slots.
	/// This exclusiveness is ensured by a scheme where ImageSystemModule class keeps its initialization
	/// and working routines private, while ImageSystem class has a friend relationship with it.
	/// The inputs of the system are implicitly represented by the inputs of reading modules.
	/// The outputs are two explicit signals: imageReady() and error().
	///
	class ImageSystem : public QObject
	{

		Q_OBJECT

	public:

		ImageSystem(ImageSystem const & other) = delete;
		ImageSystem & operator=(ImageSystem const & rhs) = delete;

		static std::unique_ptr<ImageSystem> create(std::unique_ptr<ImageSystemModule> && imageReader,
			                                       std::unique_ptr<ImageSystemModule> && imageProcessor = nullptr,
												   std::unique_ptr<ImageSystemModule> && imageWriter = nullptr);

		void run();

	Q_SIGNALS:

		void imageReady(ImagePtr image);
		void error(QString errorDescription);

	private:

		ImageSystem(std::unique_ptr<ImageSystemModule> && imageReader,
			        std::unique_ptr<ImageSystemModule> && imageProcessor = nullptr,
			        std::unique_ptr<ImageSystemModule> && imageWriter = nullptr);

	private:

		std::unique_ptr<ImageSystemModule> m_imageReader;
		std::unique_ptr<ImageSystemModule> m_imageProcessor;
		std::unique_ptr<ImageSystemModule> m_imageWriter;

	};

}
