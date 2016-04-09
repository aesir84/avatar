#pragma once

namespace avatar
{

	using ImagePtr = std::shared_ptr<QImage>;
	using MatPtr = std::shared_ptr<cv::Mat>;

}

Q_DECLARE_METATYPE(avatar::ImagePtr)
Q_DECLARE_METATYPE(avatar::MatPtr)
