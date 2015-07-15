#include "stdafx.h"

#include "webcam_capture.h"

#include "image_typedef.h"

namespace avatar
{

	WebcamCapture::WebcamCapture()
	{}

	bool WebcamCapture::present(QVideoFrame const & frame)
	{
		if (frame.isValid())
		{
			// QVideoFrame::map() is a non-const method, so we cannot call it on a const frame object.
			// Therefore, we need to clone the original frame to get a non-const object.
			//
			QVideoFrame clonedFrame(frame);

			if (clonedFrame.map(QAbstractVideoBuffer::ReadOnly))
			{
				QImage const frameImage(clonedFrame.bits(), clonedFrame.width(), clonedFrame.height(), QVideoFrame::imageFormatFromPixelFormat(clonedFrame.pixelFormat()));

				// The previously constructed QImage object doesn't copy the data provided by QVideoFrame object.
				// Instead of that, it relies on the validity of this data throughout its lifetime.
				// Unfortunately, QVideoFrame will be destructed once it leaves this scope.
				// Therefore, this situation forces to create a deep copy of the existing QImage object.
				//
				// Keeping in mind, that format RGBA8888 will later be used for QOpenGLTexture objects,
				// this circumstance can be used to create the above mentioned deep copy. Additionally,
				// the convertion is performed in advance, sparing some time in the main thread.
				//
				auto capturedImage = std::make_shared<QImage>(frameImage.convertToFormat(QImage::Format_RGBA8888));
				emit imageCaptured(capturedImage);

				clonedFrame.unmap();
				return true;
			}
		}
		return false;
	}

	QList<QVideoFrame::PixelFormat> WebcamCapture::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
	{
		if (handleType != QAbstractVideoBuffer::NoHandle)
		{
			return QList<QVideoFrame::PixelFormat>();  // no support for specialized handle types
		}

		// Return an exhaustive set of supported formats.
		//
		return QList<QVideoFrame::PixelFormat>()
			<< QVideoFrame::Format_ARGB32
			<< QVideoFrame::Format_ARGB32_Premultiplied
			<< QVideoFrame::Format_RGB32
			<< QVideoFrame::Format_RGB24
			<< QVideoFrame::Format_RGB565
			<< QVideoFrame::Format_RGB555
			<< QVideoFrame::Format_ARGB8565_Premultiplied
			<< QVideoFrame::Format_BGRA32
			<< QVideoFrame::Format_BGRA32_Premultiplied
			<< QVideoFrame::Format_BGR32
			<< QVideoFrame::Format_BGR24
			<< QVideoFrame::Format_BGR565
			<< QVideoFrame::Format_BGR555
			<< QVideoFrame::Format_BGRA5658_Premultiplied
			<< QVideoFrame::Format_AYUV444
			<< QVideoFrame::Format_AYUV444_Premultiplied
			<< QVideoFrame::Format_YUV444
			<< QVideoFrame::Format_YUV420P
			<< QVideoFrame::Format_YV12
			<< QVideoFrame::Format_UYVY
			<< QVideoFrame::Format_YUYV
			<< QVideoFrame::Format_NV12
			<< QVideoFrame::Format_NV21
			<< QVideoFrame::Format_IMC1
			<< QVideoFrame::Format_IMC2
			<< QVideoFrame::Format_IMC3
			<< QVideoFrame::Format_IMC4
			<< QVideoFrame::Format_Y8
			<< QVideoFrame::Format_Y16
			<< QVideoFrame::Format_Jpeg
			<< QVideoFrame::Format_CameraRaw
			<< QVideoFrame::Format_AdobeDng;
	}

}
