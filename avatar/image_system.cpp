#include "stdafx.h"

#include "image_system.h"

#include "imagesystem_module.h"

namespace avatar
{

	std::unique_ptr<ImageSystem> ImageSystem::create(std::unique_ptr<ImageSystemModule> && imageReader, std::unique_ptr<ImageSystemModule> && imageProcessor, std::unique_ptr<ImageSystemModule> && imageWriter)
	{
		Q_ASSERT(
			imageReader->getOperationType() == ImageSystemModule::ModuleOperation::ImageReading &&
			imageProcessor->getOperationType() == ImageSystemModule::ModuleOperation::ImageProcessing &&
			imageWriter->getOperationType() == ImageSystemModule::ModuleOperation::ImageWriting);

		return std::unique_ptr<ImageSystem>(new ImageSystem(std::move(imageReader), std::move(imageProcessor), std::move(imageWriter)));
	}

	ImageSystem::ImageSystem(std::unique_ptr<ImageSystemModule> && imageReader, std::unique_ptr<ImageSystemModule> && imageProcessor, std::unique_ptr<ImageSystemModule> && imageWriter)
		: m_imageReader(std::move(imageReader))
		, m_imageProcessor(std::move(imageProcessor))
		, m_imageWriter(std::move(imageWriter))
	{}

	void ImageSystem::run()
	{
		// TODO!!! Interconnect error signals.

		if (m_imageWriter != nullptr)
		{
			Q_ASSERT(m_imageProcessor != nullptr);

			//
			// Setup the image writer in its dedicated thread.
			//

			auto imageWriterThread = std::make_unique<QThread>();

			m_imageWriter->moveToThread(imageWriterThread.get());

			// Interconnect the writer's thread destruction chain.
			//
			QObject::connect(m_imageWriter.get(), &ImageSystemModule::destroyed, imageWriterThread.get(), &QThread::quit); // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
			QObject::connect(imageWriterThread.get(), &QThread::finished, imageWriterThread.get(), &QThread::deleteLater); // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

			// Interconnect the initialization.
			//
			QObject::connect(imageWriterThread.get(), &QThread::started, m_imageWriter.get(), &ImageSystemModule::initialize); // once the thread starts, it will trigger the work of the writer

			// Interconnect the writer's input working chain.
			//
			QObject::connect(m_imageProcessor.get(), &ImageSystemModule::imageProcessed, m_imageWriter.get(), &ImageSystemModule::processImage); // once the processor has a processed image, this image can be written

			// Interconnect the writer's destruction chain.
			//
			QObject::connect(this, &ImageSystem::destroyed, m_imageWriter.get(), &ImageSystemModule::deleteLater); // once the current object is destroyed, it will tell the worker to be destroyed as well

			// Start writing.
			//
			imageWriterThread->start();

			// Release the smartpointer's ownership of the writer's thread.
			//
			imageWriterThread.release();

			// Release the smartpointer's ownership of the writer.
			//
			m_imageWriter.release();
		}

		// This variable is used during the reader setup
		// in order to interconnect the reader's output signals.
		//
		ImageSystemModule * imageProcessor = nullptr;

		if (m_imageProcessor != nullptr)
		{
			Q_ASSERT(m_imageReader != nullptr);

			//
			// Setup the image processor in its dedicated thread.
			//

			auto imageProcessorThread = std::make_unique<QThread>();

			m_imageProcessor->moveToThread(imageProcessorThread.get());

			// Interconnect the processor's thread destruction chain.
			//
			QObject::connect(m_imageProcessor.get(), &ImageSystemModule::destroyed, imageProcessorThread.get(), &QThread::quit); // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
			QObject::connect(imageProcessorThread.get(), &QThread::finished, imageProcessorThread.get(), &QThread::deleteLater); // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

			// Interconnect the processor's input working chain.
			//
			QObject::connect(m_imageReader.get(), &ImageSystemModule::imageProcessed, m_imageProcessor.get(), &ImageSystemModule::processImage); // once the reader has an image, it will pass it to the processor

			// Interconnect the processor's output working chain.
			//
			QObject::connect(m_imageProcessor.get(), &ImageSystemModule::imageProcessed, this, &ImageSystem::imageReady);

			// Interconnect the processor's destruction chain.
			//
			QObject::connect(this, &ImageSystem::destroyed, m_imageProcessor.get(), &ImageSystemModule::deleteLater); // once the current object is destroyed, it will tell the worker to be destroyed as well

			// Start processing.
			//
			imageProcessorThread->start();

			// Release the smartpointer's ownership of the processor's thread.
			//
			imageProcessorThread.release();

			// Release the smartpointer's ownership of the processor.
			//
			imageProcessor = m_imageProcessor.release();
		}


		//
		// Setup the image reader in its dedicated thread.
		//

		Q_ASSERT(m_imageReader != nullptr);

		auto imageReaderThread = std::make_unique<QThread>();

		m_imageReader->moveToThread(imageReaderThread.get());

		// Interconnect the reader's thread destruction chain.
		//
		QObject::connect(m_imageReader.get(), &ImageSystemModule::destroyed, imageReaderThread.get(), &QThread::quit); // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(imageReaderThread.get(), &QThread::finished, imageReaderThread.get(), &QThread::deleteLater); // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Interconnect the initialization.
		//
		QObject::connect(imageReaderThread.get(), &QThread::started, m_imageReader.get(), &ImageSystemModule::initialize); // once the thread starts, it will trigger the work of the reader

		// Interconnect the reader's output working chain only if
		// there are no other units (processor,	writer) in the pipeline.
		//
		if (imageProcessor == nullptr)
		{
			QObject::connect(m_imageReader.get(), &ImageSystemModule::imageProcessed, this, &ImageSystem::imageReady);
		}

		// Interconnect the reader destruction chain.
		//
		QObject::connect(this, &ImageSystem::destroyed, m_imageReader.get(), &ImageSystemModule::deleteLater);

		// Start reading.
		//
		imageReaderThread->start();

		// Release the smartpointer's ownership of the reader's thread.
		//
		imageReaderThread.release();

		// Release the smartpointer's ownership of the reader.
		//
		m_imageReader.release();
	}

}
