#include "stdafx.h"

#include "firstpersonview_cli.h"

namespace avatar
{

	FirstPersonViewCli::FirstPersonViewCli()
		: m_initialized(false)
	{ }

	void FirstPersonViewCli::initialize()
	{
		if (m_initialized)
		{
			Q_EMIT warning("Warning : already initialized");
			return;
		}

		initializeImpl();

		m_initialized = true;
	}

	void FirstPersonViewCli::sendCommand(QByteArray const & command)
	{
		if (command == m_lastSentCommand)
		{
			return;
		}

		sendCommandImpl(command);

		m_lastSentCommand = command;
	}

}