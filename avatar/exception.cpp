#include "stdafx.h"

#include "exception.h"

namespace avatar
{

	Exception::Exception(QString const & caption, QString const & description)
		: m_caption(caption)
		, m_description(description)
	{}

	QString Exception::what() const
	{
		return m_caption + " >> description: " + m_description;
	}

}
