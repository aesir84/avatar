#include "stdafx.h"

#include "image_system.h"

#include "image_processor.h"
#include "image_reader.h"
#include "image_writer.h"

namespace avatar
{

	ImageSystem::ImageSystem(std::unique_ptr<ImageReader> && imageReader)
		: m_imageReader(std::move(imageReader))
	{}

	ImageSystem::ImageSystem(std::unique_ptr<ImageReader> && imageReader, std::unique_ptr<ImageProcessor> && imageProcessor)
		: m_imageReader(std::move(imageReader))
		, m_imageProcessor(std::move(imageProcessor))
	{}

	ImageSystem::ImageSystem(std::unique_ptr<ImageReader> && imageReader, std::unique_ptr<ImageProcessor> && imageProcessor, std::unique_ptr<ImageWriter> && imageWriter)
		: m_imageReader(std::move(imageReader))
		, m_imageProcessor(std::move(imageProcessor))
		, m_imageWriter(std::move(imageWriter))
	{}

	void ImageSystem::startHandling()
	{
		Q_ASSERT(m_imageReader != nullptr);


		//
		// Setup the working process according to the available working units.
		//

		if (m_imageProcessor != nullptr)
		{
			if (m_imageWriter != nullptr)
			{
				setupWorkingProcess(std::move(m_imageProcessor), std::move(m_imageWriter), std::move(m_imageReader));
			}
			else
			{
				setupWorkingProcess(std::move(m_imageProcessor), std::move(m_imageReader));
			}
		}
		else
		{
			setupWorkingProcess(std::move(m_imageReader));
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// TODO: setupWorkingProcess() methods have lots of code duplication. This needs to be refactored.
	//

	void ImageSystem::setupWorkingProcess(std::unique_ptr<ImageReader> && imageReader) const
	{

		//
		// Setup the image reader in its dedicated thread.
		//

		auto imageReaderThread = std::make_unique<QThread>();

		imageReader->moveToThread(imageReaderThread.get());

		// Interconnect the reader's thread destruction chain.
		//
		QObject::connect(imageReader.get(), &ImageReader::destroyed, imageReaderThread.get(), &QThread::quit);               // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(imageReaderThread.get(), &QThread::finished, imageReaderThread.get(), &QThread::deleteLater);       // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Interconnect the working chain.
		//
		QObject::connect(imageReaderThread.get(), &QThread::started, imageReader.get(), &ImageReader::startReading);         // once the thread starts, it will trigger the work of the reader
		QObject::connect(imageReader.get(), &ImageReader::imageRead, this, &ImageSystem::imageReady);                       // once the reader has an image, it will pass it to the processor

		// Interconnect the reader destruction chain.
		//
		QObject::connect(this, &ImageSystem::destroyed, imageReader.get(), &ImageReader::deleteLater);


		//
		// Start working.
		//

		imageReaderThread->start();


		//
		// Release the smartpointer's ownership of the thread.
		//

		imageReaderThread.release();


		//
		// Release the smartpointer's ownership of the reader.
		//

		imageReader.release();
	}

	void ImageSystem::setupWorkingProcess(std::unique_ptr<ImageProcessor> && imageProcessor, std::unique_ptr<ImageReader> && imageReader) const
	{

		//
		// Setup the image processor in its dedicated thread.
		//

		auto imageProcessorThread = std::make_unique<QThread>();

		imageProcessor->moveToThread(imageProcessorThread.get());

		// Interconnect the processor's thread destruction chain.
		//
		QObject::connect(imageProcessor.get(), &ImageProcessor::destroyed, imageProcessorThread.get(), &QThread::quit);      // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(imageProcessorThread.get(), &QThread::finished, imageProcessorThread.get(), &QThread::deleteLater); // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Interconnect the working chain.
		//
		QObject::connect(imageProcessor.get(), &ImageProcessor::imageProcessed, this, &ImageSystem::imageReady);

		// Interconnect the processor's destruction chain.
		//
		QObject::connect(this, &ImageSystem::destroyed, imageProcessor.get(), &ImageProcessor::deleteLater);                // once the current object is destroyed, it will tell the worker to be destroyed as well


		//
		// Setup the image reader in its dedicated thread.
		//

		auto imageReaderThread = std::make_unique<QThread>();

		imageReader->moveToThread(imageReaderThread.get());

		// Interconnect the reader's thread destruction chain.
		//
		QObject::connect(imageReader.get(), &ImageReader::destroyed, imageReaderThread.get(), &QThread::quit);               // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(imageReaderThread.get(), &QThread::finished, imageReaderThread.get(), &QThread::deleteLater);       // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Interconnect the working chain.
		//
		QObject::connect(imageReaderThread.get(), &QThread::started, imageReader.get(), &ImageReader::startReading);         // once the thread starts, it will trigger the work of the reader
		QObject::connect(imageReader.get(), &ImageReader::imageRead, imageProcessor.get(), &ImageProcessor::processImage);   // once the reader has an image, it will pass it to the processor

		// Interconnect the reader destruction chain.
		//
		QObject::connect(this, &ImageSystem::destroyed, imageReader.get(), &ImageReader::deleteLater);

		
		//
		// Start working.
		//

		imageProcessorThread->start();
		imageReaderThread->start();


		//
		// Release the smartpointers' ownership of the threads.
		//

		imageProcessorThread.release();
		imageReaderThread.release();


		//
		// Release the smartpointers' ownership of the workers.
		//

		imageProcessor.release();
		imageReader.release();
	}

	void ImageSystem::setupWorkingProcess(std::unique_ptr<ImageProcessor> && imageProcessor, std::unique_ptr<ImageWriter> && imageWriter, std::unique_ptr<ImageReader> && imageReader) const
	{

		//
		// Setup the image processor in its dedicated thread.
		//

		auto imageProcessorThread = std::make_unique<QThread>();

		imageProcessor->moveToThread(imageProcessorThread.get());

		// Interconnect the processor's thread destruction chain.
		//
		QObject::connect(imageProcessor.get(), &ImageProcessor::destroyed, imageProcessorThread.get(), &QThread::quit);      // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(imageProcessorThread.get(), &QThread::finished, imageProcessorThread.get(), &QThread::deleteLater); // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Interconnect the working chain.
		//
		QObject::connect(imageProcessor.get(), &ImageProcessor::imageProcessed, this, &ImageSystem::imageReady);

		// Interconnect the processor's destruction chain.
		//
		QObject::connect(this, &ImageSystem::destroyed, imageProcessor.get(), &ImageProcessor::deleteLater);                // once the current object is destroyed, it will tell the worker to be destroyed as well


		//
		// Setup the image writer in its dedicated thread.
		//

		auto imageWriterThread = std::make_unique<QThread>();

		imageWriter->moveToThread(imageWriterThread.get());

		// Interconnect the writer's thread destruction chain.
		//
		QObject::connect(imageWriter.get(), &ImageWriter::destroyed, imageWriterThread.get(), &QThread::quit);         // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(imageWriterThread.get(), &QThread::finished, imageWriterThread.get(), &QThread::deleteLater); // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Interconnect the working chain.
		//
		QObject::connect(imageWriterThread.get(), &QThread::started, imageWriter.get(), &ImageWriter::startWriting);          // once the thread starts, it will trigger the work of the writer
		QObject::connect(imageProcessor.get(), &ImageProcessor::imageProcessed, imageWriter.get(), &ImageWriter::writeImage); // once the processor has a processed image, this image can be written

		// Interconnect the writer's destruction chain.
		//
		QObject::connect(this, &ImageSystem::destroyed, imageWriter.get(), &ImageWriter::deleteLater);                // once the current object is destroyed, it will tell the worker to be destroyed as well


		//
		// Setup the image reader in its dedicated thread.
		//

		auto imageReaderThread = std::make_unique<QThread>();

		imageReader->moveToThread(imageReaderThread.get());

		// Interconnect the reader's thread destruction chain.
		//
		QObject::connect(imageReader.get(), &ImageReader::destroyed, imageReaderThread.get(), &QThread::quit);               // once the worker is destroyed, it will tell the thread to quit [thread's loop is still running]
		QObject::connect(imageReaderThread.get(), &QThread::finished, imageReaderThread.get(), &QThread::deleteLater);       // [thread's loop is still running] when the thread finishes, it will tell its object to delete itself

		// Interconnect the working chain.
		//
		QObject::connect(imageReaderThread.get(), &QThread::started, imageReader.get(), &ImageReader::startReading);         // once the thread starts, it will trigger the work of the reader
		QObject::connect(imageReader.get(), &ImageReader::imageRead, imageProcessor.get(), &ImageProcessor::processImage);   // once the reader has an image, it will pass it to the processor

		// Interconnect the reader destruction chain.
		//
		QObject::connect(this, &ImageSystem::destroyed, imageReader.get(), &ImageReader::deleteLater);


		//
		// Start working.
		//

		imageProcessorThread->start();
		imageWriterThread->start();
		imageReaderThread->start();


		//
		// Release the smartpointers' ownership of the threads.
		//

		imageProcessorThread.release();
		imageWriterThread.release();
		imageReaderThread.release();


		//
		// Release the smartpointers' ownership of the workers.
		//

		imageProcessor.release();
		imageWriter.release();
		imageReader.release();
	}

}
