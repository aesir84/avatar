#include "stdafx.h"

#include "ovrvision_imagereader.h"

#include "exception.h"

namespace avatar
{
	OVRVisionImageReader::OVRVisionImageReader()
		: m_stopped(false)
	{
		if (m_vision.Open(0, OVR::OV_CAMVR_FULL) <= 0)
		{
			throw Exception("OVRVision", "failed to open the camera");
		}
	}

	OVRVisionImageReader::~OVRVisionImageReader()
	{
		m_stopped = true;
		m_vision.Close();
	}

	void OVRVisionImageReader::read()
	{
		int const imageWidth = m_vision.GetCamWidth();
		int const imageHeight = m_vision.GetCamHeight();

		while (!m_stopped)
		{
			auto leftEyeImage = std::make_shared<cv::Mat>(imageHeight, imageWidth, CV_8UC4);
			auto rightEyeImage = std::make_shared<cv::Mat>(imageHeight, imageWidth, CV_8UC4);

			m_vision.PreStoreCamData(OVR::OV_CAMQT_DMS);

			m_vision.GetCamImageBGRA(leftEyeImage->data, OVR::OV_CAMEYE_LEFT);
			m_vision.GetCamImageBGRA(rightEyeImage->data, OVR::OV_CAMEYE_RIGHT);

			Q_EMIT leftImageCaptured(leftEyeImage);
			Q_EMIT rightImageCaptured(rightEyeImage);
		}
	}
}
