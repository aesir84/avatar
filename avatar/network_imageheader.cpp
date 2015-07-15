#include "stdafx.h"

#include "network_imageheader.h"

namespace avatar
{

	namespace
	{
		using Signature = quint32;
		Signature const ImageHeaderSignature = 0xC5362A99;
	}

	NetworkImageHeader::NetworkImageHeader(ImageSize imageSize)
		: m_imageSize(imageSize)
	{}

	QDataStream & operator>>(QDataStream & dataStream, NetworkImageHeader & remoteImageHeader)
	{
		Signature dataStreamSignature;

		dataStream >> dataStreamSignature;

		if (dataStreamSignature != ImageHeaderSignature)
		{
			dataStream.setStatus(QDataStream::ReadCorruptData);
			return dataStream;
		}

		dataStream >> remoteImageHeader.m_imageSize;

		return dataStream;
	}

	QDataStream & operator<<(QDataStream & dataStream, NetworkImageHeader const & remoteImageHeader)
	{
		dataStream
			<< ImageHeaderSignature
			<< remoteImageHeader.m_imageSize;

		return dataStream;
	}

}
