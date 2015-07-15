#pragma once

namespace avatar
{

	using ImagePtr = std::shared_ptr<QImage>;

}

Q_DECLARE_METATYPE(avatar::ImagePtr)
