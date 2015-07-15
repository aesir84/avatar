#include "stdafx.h"

#include "network_imagesettings.h"

namespace avatar
{

	namespace
	{
		char const * const ImagePngFormat = "PNG";
		char const * const ImageJpgFormat = "JPG";
	}

	char const * const NetworkImageSettings::m_imageFormat = ImageJpgFormat;
	int const NetworkImageSettings::m_imageQuality = 60;

	char const * const NetworkImageSettings::getImageFormat()
	{
		return m_imageFormat;
	}

	int NetworkImageSettings::getImageQuality()
	{
		return m_imageQuality;
	}

}
