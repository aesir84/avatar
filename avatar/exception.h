#pragma once

#include "stdafx.h"

namespace avatar
{

	class Exception
	{

	public:

		Exception(QString const & caption, QString const & description);
		QString what() const;

	private:

		QString m_caption;
		QString m_description;

	};

}
