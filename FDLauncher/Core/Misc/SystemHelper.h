/* -------------------------------------------------------------------------
//  File Name   :   SystemHelper.h
//  Author      :   zhang fan
//  Create Time :   2015/05/23 13:32:02
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __SYSTEMHELPER_H__
#define __SYSTEMHELPER_H__



//-------------------------------------------------------------------------

namespace SystemHelper
{
	CPath	getMainAppPath();
	CPath	getModulePath();
	CPath	getTempPath();
	HANDLE	shellExecute(const CPath& exePath, const CString& params, BOOL elevate);
	BOOL	isElevated();
	BOOL	getFileVersion(const CString& filePath, CString& version);
	BOOL	isUnderXP();
	BOOL	waitProcess(DWORD processId);
	BOOL	getDiskFreeSpace(LPCWSTR path, UInt64& space);
	BOOL	removeOpenFileWarning(const CPath& filePath);
}

//--------------------------------------------------------------------------
#endif /* __SYSTEMHELPER_H__ */
