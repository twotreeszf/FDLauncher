/* -------------------------------------------------------------------------
//  File Name   :   CommandLineHandler.h
//  Author      :   zhang fan
//  Create Time :   2015/05/29 16:30:22
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __COMMANDLINEHANDLER_H__
#define __COMMANDLINEHANDLER_H__

#include "../Misc/CommandLine.h"

//-------------------------------------------------------------------------

namespace CommandLineHandler
{
	BOOL handleCommandLine(CCommandLineOption* commandLine);

namespace Private
{
	void handleDeleteMe(const CString& filePath, DWORD processId);
}

}

//--------------------------------------------------------------------------
#endif /* __COMMANDLINEHANDLER_H__ */
