#include "stdafx.h"

#include "app_environment.h"

#include "exception.h"

namespace avatar
{

	AppEnvironment::AppEnvironment(int & argc, char ** argv)
		: QGuiApplication(argc, argv)
	{}

	bool AppEnvironment::notify(QObject * object, QEvent * ev)
	{
		try
		{
			return QGuiApplication::notify(object, ev);
		}
		catch (Exception const & e)
		{
			std::wcout << e.what().toStdWString() << std::endl;
			exit(EXIT_FAILURE);
		}
		catch (...)
		{
			std::wcout << L"Runtime error >> description: unknown" << std::endl;
			exit(EXIT_FAILURE);
		}
		return false;
	}

}