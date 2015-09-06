#include "stdafx.h"

#include "app_environment.h"
#include "firstpersonview_app.h"
#include "cmdline_options.h"
#include "cmdline_parser.h"
#include "colorcube_app.h"
#include "exception.h"
#include "texcube_app.h"
#include "videostream_clientapp.h"
#include "videostream_serverapp.h"
#include "webcam_monoapp.h"
#include "webcam_stereoapp.h"

#include "firstpersonview_serialcli.h"
#include "image_system.h"
#include "webcam_imageprocessor.h"
#include "webcam_imagereader.h"

int main(int argc, char ** argv)
{
	int status = EXIT_FAILURE;

	try
	{
		avatar::AppEnvironment env(argc, argv);
		avatar::cmdline_utils::Parser cmdlineParser;
		std::unique_ptr<avatar::RiftApp> app;

		auto cmdlineParserResult = cmdlineParser.parseOptions();

		switch (cmdlineParserResult.first)
		{

			case avatar::cmdline_utils::Parser::Status::LaunchColorCubeDemo:
			{
				std::wcout << L"Info >> description: launching color cube demo" << std::endl;
				app.reset(new avatar::ColorCubeApp);
			}
			break;

			case avatar::cmdline_utils::Parser::Status::LaunchTextureCubeDemo:
			{
				std::wcout << L"Info >> description: launching texture cube demo" << std::endl;
				app.reset(new avatar::TexCubeApp);
			}
			break;

			case avatar::cmdline_utils::Parser::Status::LaunchMonoWebcamDemo:
			{
				std::wcout << L"Info >> description: launching mono webcam demo" << std::endl;
				app.reset(new avatar::WebcamMonoApp);
			}
			break;

			case avatar::cmdline_utils::Parser::Status::LaunchStereoWebcamDemo:
			{
				std::wcout << L"Info >> description: launching stereo webcam demo" << std::endl;
				app.reset(new avatar::WebcamStereoApp);
			}
			break;

			case avatar::cmdline_utils::Parser::Status::LaunchClientDemo:
			{
				QString const serverAddressString = cmdlineParser.value(avatar::cmdline_utils::launchClientDemo);
				QHostAddress serverAddress(serverAddressString);

				std::wcout << L"Info >> description: launching client demo with server IP address " << serverAddressString.toStdWString() << std::endl;

				app.reset(new avatar::VideoStreamClientApp(serverAddress));
			}
			break;

			case avatar::cmdline_utils::Parser::Status::LaunchServerDemo:
			{
				std::wcout << L"Info >> description: launching server demo" << std::endl;
				app.reset(new avatar::VideoStreamServerApp);
			}
			break;

			case avatar::cmdline_utils::Parser::Status::LaunchFirstPersonViewDemo:
			{
				std::wcout << L"Info >> description: launching FPV demo" << std::endl;


				//
				// Find arduino serial port.
				//

				QList<QSerialPortInfo> const ports = QSerialPortInfo::availablePorts();
				QList<QSerialPortInfo>::const_iterator arduinoPort = ports.cend();

				for (auto it = ports.cbegin(); it != ports.cend(); ++it)
				{
					QString const description = it->description();

					if (description.contains("arduino", Qt::CaseInsensitive))
					{
						arduinoPort = it;
						break;
					}
				}

				if (arduinoPort == ports.cend())
				{
					std::wcout << L"Error >> description: failed to find arduino serial port" << std::endl;
					return EXIT_FAILURE;
				}


				//
				// Find a webcam.
				//

				auto const cameras = QCameraInfo::availableCameras();

				if (cameras.empty())
				{
					std::wcout << L"Error >> description: failed to find webcam devices" << std::endl;
					return EXIT_FAILURE;
				}


				//
				// Setup both the interpreter for the arduino port and the camera.
				//
				
				auto commandInterpreter = avatar::FirstPersonViewSerialCli::create(*arduinoPort, QSerialPort::Baud115200, QByteArray("Servo>"), 50);
				auto imageSystem = avatar::ImageSystem::create(std::make_unique<avatar::WebcamImageReader>(cameras[0]), std::make_unique<avatar::WebcamImageProcessor>());

				app.reset(new avatar::FirstPersonViewApp(std::move(commandInterpreter), std::move(imageSystem)));
			}
			break;

			case avatar::cmdline_utils::Parser::Status::ShowHelp:
			{
				cmdlineParser.showHelp(EXIT_SUCCESS); // showHelp() will exit the application
			}
			break;

			case avatar::cmdline_utils::Parser::Status::Error:
			{
				std::wcout << cmdlineParserResult.second.toStdWString() << std::endl;
				cmdlineParser.showHelp(EXIT_FAILURE); // showHelp() will exit the application
			}
			break;

			default:
			{
				Q_ASSERT(false);
				cmdlineParser.showHelp(EXIT_FAILURE); // showHelp() will exit the application
			}

		}

		app->show();
		status = env.exec();

	}
	catch (avatar::Exception const & e)
	{
		std::wcout << e.what().toStdWString() << std::endl;
	}

	return status;
}