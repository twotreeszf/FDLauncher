/* -------------------------------------------------------------------------
//  File Name   :   CommandLineHandler.cpp
//  Author      :   zhang fan
//  Create Time :   2015/05/29 16:30:09
//  Description :
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "CommandLineHandler.h"
#include "CommandLineDef.h"
#include "..\Misc\SystemHelper.h"

//-------------------------------------------------------------------------

namespace CommandLineHandler
{
	BOOL handleCommandLine(CCommandLineOption* commandLine)
	{
		using namespace Private;
		if (commandLine->HasSwitch(CommandLine::DELETE_ME))
		{
			CString path;
			commandLine->GetSwitchParam(CommandLine::PATH, path);
			CString processId;
			commandLine->GetSwitchParam(CommandLine::PROCESS_ID, processId);

			if (path.GetLength() && processId.GetLength())
			{
				handleDeleteMe(path, _wtoi(processId));
				return TRUE;
			}
		}
		else if (commandLine->HasSwitch(CommandLine::WAIT_EXIT))
		{
			CString processId;
			commandLine->GetSwitchParam(CommandLine::PROCESS_ID, processId);

			if (processId.GetLength())
			{
				SystemHelper::waitProcess(_wtoi(processId));
				return FALSE;
			}

		}

		return FALSE;
	}

namespace Private
{
	void handleDeleteMe(const CString& filePath, DWORD processId)
	{
		SystemHelper::waitProcess(processId);
		DeleteFile(filePath);
	}
}
}


//--------------------------------------------------------------------------         
