#pragma once

namespace avatar
{

	namespace cmdline_utils
	{

		class Parser : public QCommandLineParser
		{

		public:

			enum class Status
			{
				LaunchColorCubeDemo,
				LaunchTextureCubeDemo,
				LaunchMonoWebcamDemo,
				LaunchStereoWebcamDemo,
				LaunchClientDemo,
				LaunchServerDemo,
				LaunchArduinoDemo,
				ShowHelp,
				Error,
			};

			using Result = std::pair<Status, QString>;

		public:

			Parser();
			Result parseOptions();

		private:

			QCommandLineOption m_helpOption;

		};

	}

}
