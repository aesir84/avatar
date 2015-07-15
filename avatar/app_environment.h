#pragma once

namespace avatar
{

	/// \brief Environment for running apps
	///
	/// Currently, the main reason for subclassing QGuiApplication
	/// is to override notify() method. By doing so, it is possible
	/// to take control of any C++ exceptions thrown while replying
	/// to system notifications.
	///
	class AppEnvironment : public QGuiApplication
	{

		Q_OBJECT

	public:

		AppEnvironment(int & argc, char ** argv);

		virtual bool notify(QObject * object, QEvent * ev) override;

	};

}
