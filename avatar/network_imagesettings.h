#pragma once

namespace avatar
{

	class NetworkImageSettings
	{

	public:

		static char const * const getImageFormat();
		static int getImageQuality();

	private:

		static char const * const m_imageFormat;
		static int const m_imageQuality;

	};

}
