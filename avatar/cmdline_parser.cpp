#include "stdafx.h"

#include "cmdline_parser.h"

#include "cmdline_options.h"

namespace avatar
{

	namespace cmdline_utils
	{

		Parser::Parser()
			: m_helpOption(addHelpOption())
		{
			setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
			addOption(launchColorCubeDemo);
			addOption(launchTextureCubeDemo);
			addOption(launchMonoWebcamDemo);
			addOption(launchStereoWebcamDemo);
			addOption(launchClientDemo);
			addOption(launchServerDemo);
			addOption(launchArduinoDemo);
		}

		Parser::Result Parser::parseOptions()
		{
			if (!parse(QCoreApplication::arguments()))
			{
				QString const errorDescription = "Command line error >> description: " + errorText();
				return std::make_pair(Status::Error, errorDescription);
			}

			if (isSet(m_helpOption))
			{
				return std::make_pair(Status::ShowHelp, QString());
			}

			if (isSet(launchColorCubeDemo))
			{
				return std::make_pair(Status::LaunchColorCubeDemo, QString());
			}

			if (isSet(launchTextureCubeDemo))
			{
				return std::make_pair(Status::LaunchTextureCubeDemo, QString());
			}

			if (isSet(launchMonoWebcamDemo))
			{
				return std::make_pair(Status::LaunchMonoWebcamDemo, QString());
			}

			if (isSet(launchStereoWebcamDemo))
			{
				return std::make_pair(Status::LaunchStereoWebcamDemo, QString());
			}

			if (isSet(launchClientDemo))
			{
				QStringList const serverAddressOptionValues = values(launchClientDemo);

				if (serverAddressOptionValues.isEmpty())
				{
					return std::make_pair(Status::Error, "Command line error >> description: no server address specified");
				}

				if (serverAddressOptionValues.size() > 1)
				{
					return std::make_pair(Status::Error, "Command line error >> description: too many server addresses specified");
				}

				return std::make_pair(Status::LaunchClientDemo, QString());
			}

			if (isSet(launchServerDemo))
			{
				return std::make_pair(Status::LaunchServerDemo, QString());
			}

			if (isSet(launchArduinoDemo))
			{
				return std::make_pair(Status::LaunchArduinoDemo, QString());
			}

			return std::make_pair(Status::Error, "Command line error >> description: unknown option");
		}

	}

}
