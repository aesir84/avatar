#pragma once

namespace avatar
{

	struct NetworkImageHeader
	{
		using ImageSize = qint64;

		NetworkImageHeader(ImageSize imageSize = 0);

		ImageSize m_imageSize;
	};

	QDataStream & operator>>(QDataStream & dataStream, NetworkImageHeader & remoteImageHeader);
	QDataStream & operator<<(QDataStream & dataStream, NetworkImageHeader const & remoteImageHeader);

}
