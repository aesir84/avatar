#pragma once

#include "imagesystem_typedefs.h"

namespace avatar
{
	class OVRVisionImageReader : public QObject
	{
		Q_OBJECT

	public:
		OVRVisionImageReader();
		~OVRVisionImageReader();

	Q_SIGNALS:
		void leftImageCaptured(MatPtr image);
		void rightImageCaptured(MatPtr image);

	public:
		void read();

	private:
		OVR::OvrvisionPro m_vision;
		std::atomic<bool> m_stopped;
	};
}
