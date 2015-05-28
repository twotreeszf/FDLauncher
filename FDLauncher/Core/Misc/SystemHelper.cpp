/* -------------------------------------------------------------------------
//  File Name   :   SystemHelper.cpp
//  Author      :   zhang fan
//  Create Time :   2015/05/23 13:32:07
//  Description :
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include <shlobj.h>
#include <shellapi.h>

#pragma comment(lib, "Version.lib")

//-------------------------------------------------------------------------

namespace SystemHelper
{
	CPath getMainAppPath()
	{
		CString programFiles;
		SHGetSpecialFolderPath(0, programFiles.GetBufferSetLength(MAX_PATH), CSIDL_PROGRAM_FILES, FALSE);

		CPath path = programFiles;
		path.Append(L"狸米学习");
		path.Append(L"LMXX.exe");

		return path;
	}

	CPath getModulePath()
	{
		CString path;
		GetModuleFileName(GetModuleHandle(NULL), path.GetBufferSetLength(MAX_PATH), MAX_PATH);
		return path;
	}

	CPath getTempPath()
	{
		CString path;
		GetTempPath(MAX_PATH, path.GetBufferSetLength(MAX_PATH));

		return path;
	}

	HANDLE shellExecute(const CPath& exePath, const CString& params, BOOL runUAC)
	{
		SHELLEXECUTEINFO shExInfo = { 0 };
		shExInfo.cbSize			= sizeof(shExInfo);
		shExInfo.fMask			= SEE_MASK_NOCLOSEPROCESS;
		shExInfo.lpVerb			= runUAC ? L"runas" : L"";
		shExInfo.lpFile			= exePath;
		shExInfo.lpParameters	= params;
		shExInfo.nShow			= SW_SHOW;

		BOOL ret = ShellExecuteEx(&shExInfo);
		return ret ? shExInfo.hProcess : NULL;
	}

	BOOL getFileVersion(const CString& filePath, CString& version)
	{
		BOOL ret = TRUE;
		{
			DWORD               dwSize = 0;
			VS_FIXEDFILEINFO    *pFileInfo = NULL;
			UINT                puLenFileInfo = 0;

			dwSize = GetFileVersionInfoSize(filePath, NULL);
			ERROR_CHECK_BOOLEX(dwSize, ret = FALSE);

			CTempBuffer<BYTE> versionInfo(dwSize);
			ret = GetFileVersionInfo(filePath, 0, dwSize, versionInfo);
			ERROR_CHECK_BOOL(ret);

			ret = VerQueryValue(versionInfo, L"\\", (LPVOID*)&pFileInfo, &puLenFileInfo);
			ERROR_CHECK_BOOL(ret);

			version.Format(L"%d.%d.%d.%d",
				(pFileInfo->dwFileVersionMS >> 16) & 0xffff,
				(pFileInfo->dwFileVersionMS >> 0) & 0xffff,
				(pFileInfo->dwFileVersionLS >> 16) & 0xffff,
				(pFileInfo->dwFileVersionLS >> 0) & 0xffff);

		}

	Exit0:
		return ret;
	}

	BOOL isUnderXP()
	{
		OSVERSIONINFO OsInfo;
		ZeroMemory(&OsInfo, sizeof(OSVERSIONINFO));
		OsInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx(&OsInfo);
		if (OsInfo.dwMajorVersion < 6)
		{
			return TRUE;
		}//非Vista以上版本

		return FALSE;
	}

}

//--------------------------------------------------------------------------         
