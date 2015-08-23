#pragma once

namespace avatar
{

	namespace cmdline_utils
	{

		QCommandLineOption const launchColorCubeDemo(
		{ "c", "colorcube" },
		"Launch the color cube demo.");

		QCommandLineOption const launchTextureCubeDemo(
		{ "t", "texturecube" },
		"Launch the texture cube demo.");

		QCommandLineOption const launchMonoWebcamDemo(
		{ "m", "monowebcam" },
		"Launch the mono webcam demo. A webcam must be already plugged in.");

		QCommandLineOption const launchStereoWebcamDemo(
		{ "s", "stereowebcam" },
		"Launch the stereo webcam demo. Two webcams must be already plugged in.");

		QCommandLineOption const launchClientDemo(
		{ "l", "client" },
		"Launch the stereo webcam demo with remote server. Two webcams must be already plugged in. Specify the IP <address> of the server you wish to send the webcam video stream to.",
		"address");

		QCommandLineOption const launchServerDemo(
		{ "r", "server" },
		"Launch the stereo server demo. The server shall receive a video stream from a remote client.");

		QCommandLineOption const launchFirstPersonViewDemo(
		{ "f", "firstpersonview" },
		"Launch the first person view demo. All the required components must be already attached to the system");

	}

}
