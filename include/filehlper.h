/* -------------------------------------------------------------------------
//	文件名		：	filehlper.h
//	创建者		：	冰峰
//	创建时间	：	2008-11-6 19:42:36
//	功能描述	：	文件/文件夹辅助类
//
//	$Id: $
// -----------------------------------------------------------------------*/
#ifndef __FILEHLPER_H__
#define __FILEHLPER_H__

#include "string_hlp.h"
#include <io.h>
#include <wchar.h>
#include <stack>
#include <queue>
#include <bitset>
#include <set>
#include "nocasestring.h"
#include <shlobj.h>
#include <Shellapi.h>
#include <Sddl.h>
#include "base.h"
#include "nocasestring.h"

namespace FileHlp {

	//////////////////////////////////////////////////////////////////////////
	// 枚举文件
	typedef enum { EnumFileType_File, EnumFileType_EnterDirectory, EnumFileType_LeaveDirectory } EnumFileType;
	typedef BOOL (__stdcall *PFN_ENUM_FILE)(LPCWSTR szPath, LPCWSTR szName, _wfinddata_t *pfd, LPVOID user_data);
	typedef BOOL (__stdcall *PFN_ENUM_FILE2)(EnumFileType eEnumFileType, LPCWSTR szPath, LPCWSTR szFile, _wfinddata_t *pfd, LPVOID user_data);

	typedef std::pair<BOOL, BOOL>				_PAIR_DELETE_;	// 是否删除到回收站，是否忽略删除中的错误

	//////////////////////////////////////////////////////////////////////////
	// 
	inline BOOL _EnumFileBFS(LPCWSTR _szCurrentPath, PFN_ENUM_FILE pfnCallBack, LPVOID user_data, 
		BOOL bIncludeSubNode, LPCWSTR szMask, std::vector<CString>* pvecLongPaths)
	{
		CPath pathRoot = _szCurrentPath;
		ASSERT(pathRoot.IsDirectory());

		std::set<CNoCaseString> setFilters;	// 这里用 CNoCaseString 避免大小写无法匹配
		setFilters.insert(L"RECYCLER");
		setFilters.insert(L"$RECYCLE.BIN");
		setFilters.insert(L"Recycled");
		setFilters.insert(L"System Volume Information");

		std::queue<CPath> quTreeNodes;
		quTreeNodes.push(pathRoot);

		BOOL bRet = TRUE;
		{
			while (quTreeNodes.size())
			{
				CPath pathCurNode = quTreeNodes.front();
				quTreeNodes.pop();
				ASSERT(pathCurNode.IsDirectory());

				CPath pathFindMask = pathCurNode;
				pathFindMask.Append(szMask);

				_wfinddata_t	fd		= {0};
				intptr_t		hFind	= _wfindfirst(pathFindMask, &fd);
				int				nRet	= 0;
				while(-1 != hFind && -1 != nRet)
				{

					if (fd.attrib & _A_SUBDIR)
					{
						if (CString(fd.name) != L"." && CString(fd.name) != L"..")
						{
							// Special directory exists only in the root of the disk
							if (!pathCurNode.IsRoot() || setFilters.find(fd.name) == setFilters.end())
							{
								// check long path
								CString& strCurNode = (CString&)pathCurNode;
								if (pvecLongPaths && (strCurNode.GetLength() + wcslen(fd.name)) > MAX_PATH )
								{
									CString strLongPath;
									strLongPath += strCurNode;
									strLongPath += fd.name;
									pvecLongPaths->push_back(strLongPath);
									goto LoopEnd;	
								}

								bRet = pfnCallBack(pathCurNode, fd.name, &fd, user_data);
								if (!bRet)
									break;

								if (bIncludeSubNode)
								{
									CPath pathSubNode = pathCurNode;
									pathSubNode.Append(fd.name);
									pathSubNode.AddBackslash();
									quTreeNodes.push(pathSubNode);
								}
							}
						}			
					}
					else
					{
						bRet = pfnCallBack(pathCurNode, fd.name, &fd, user_data);
						if(!bRet)
							break;
					}

LoopEnd:
					nRet = _wfindnext(hFind, &fd);
				}

				_findclose(hFind);
				CHECK_BOOL(bRet);
			}
		}

Exit0:
		return bRet;
	}

	inline void UniformPath(CPath &path, BOOL bPrefix = TRUE, BOOL bSuffix = FALSE)
	{
		CString strTemp;
		if (bPrefix)	// add prefix
		{
			if (path.m_strPath.IsEmpty() || path.m_strPath.GetAt(0) != '\\')
				path.m_strPath = L"\\" + path.m_strPath;
		}
		else			// del prefix
		{
			if (!path.m_strPath.IsEmpty() && path.m_strPath.GetAt(0) == '\\')
			{
				path.m_strPath = path.m_strPath.Right(path.m_strPath.GetLength() - 1);
			}
		}

		//
		if (bSuffix)
			path.AddBackslash();
		else
		{
			//		path.RemoveBackslash();		// 如果是K:\，会去不掉，所以只能自己去
			if (!path.m_strPath.IsEmpty() && path.m_strPath.GetAt(path.m_strPath.GetLength() - 1) == '\\')
				path.m_strPath = path.m_strPath.Left(path.m_strPath.GetLength() - 1);
		}
	}

	inline CPath UniformPath(int, CPath path, BOOL bPrefix = TRUE, BOOL bSuffix = FALSE)
	{
		UniformPath(path, bPrefix, bSuffix);
		return path;
	}

	inline BOOL IsSubDir(LPCWSTR szTestPath, LPCWSTR szParentMaybe, BOOL bIncludeSame = FALSE)
	{
		CPath pathTest(szTestPath), pathParentMaybe(szParentMaybe);
		UniformPath(pathTest, TRUE, bIncludeSame);
		UniformPath(pathParentMaybe, TRUE, TRUE);
		pathTest.m_strPath.MakeLower();
		pathParentMaybe.m_strPath.MakeLower();
		LPCWSTR szPos = String_Hlp::wcsstr_nocase(pathTest, pathParentMaybe);
		return szPos == (LPCWSTR)pathTest;
	}

	inline CString GetFileName(LPCWSTR szFullPath)
	{
		ASSERT(szFullPath);
		CPath path(szFullPath);
		path.RemoveBackslash();
		LPCWSTR szName = wcsrchr(path, '\\');
		return szName ? szName + 1: path;
	}

	inline CString GetFolderName(LPCWSTR szFullPath)
	{
		return GetFileName(szFullPath);
	}


	inline BOOL EnumFileBFS(LPCWSTR _szCurrentPath, PFN_ENUM_FILE pfnCallBack, LPVOID user_data, 
		BOOL bIncludeSubNode = TRUE, std::vector<CString>* pvecLongPaths = NULL)
	{
		return _EnumFileBFS(_szCurrentPath, pfnCallBack, user_data, bIncludeSubNode, L"*.*", pvecLongPaths);
	}

	inline BOOL EnumFileMatchSpec(LPCWSTR _szCurrentPath, PFN_ENUM_FILE pfnCallBack, LPVOID user_data, LPCWSTR szMask = L"*.*")
	{
		return _EnumFileBFS(_szCurrentPath, pfnCallBack, user_data, FALSE, szMask, NULL);
	}

// 	inline BOOL EnumFileBFSForSelective(LPCWSTR _szCurrentPath, PFN_ENUM_FILE pfnCallBack, LPVOID user_data, 
// 		BOOL bIncludeSubNode, PFN_TransportCallBack pfnCallCancelback, LPVOID pCancleData)
// 	{
// 		return _EnumFileBFSForSelective(_szCurrentPath, pfnCallBack, user_data, bIncludeSubNode, L"*.*", pfnCallCancelback, pCancleData);
// 	}

	//////////////////////////////////////////////////////////////////////////
	// 深度优先
	inline BOOL EnumFile2(LPCWSTR _szCurrentPath, LPCWSTR szSpec, PFN_ENUM_FILE2 pfnCallBack, LPVOID user_data)
	{
		BOOL bRet = TRUE;
		WCHAR szCurrentPath[MAX_PATH];
		wcscpy(szCurrentPath, _szCurrentPath);
		if (*(szCurrentPath + wcslen(szCurrentPath) - 1) != '\\')
			wcscat(szCurrentPath, L"\\");

		bRet = pfnCallBack(EnumFileType_EnterDirectory, szCurrentPath, NULL, NULL, user_data);
		if (!bRet)
			goto Exit0;

		WCHAR szEnumPath[MAX_PATH];
		wcscpy(szEnumPath, szCurrentPath);
		wcscat(szEnumPath, szSpec);

		_wfinddata_t fd;
		long hFind = _wfindfirst((LPWSTR)szEnumPath, &fd);
		while (hFind != -1)
		{
			if (fd.attrib & _A_SUBDIR)
			{
				if (wcscmp(fd.name, L".") && wcscmp(fd.name, L".."))
				{
					WCHAR szTemp[MAX_PATH];
					wcscpy(szTemp, szCurrentPath);
					wcscat(szTemp, fd.name);
					BOOL bRet = EnumFile2(szTemp, szSpec, pfnCallBack, user_data);
					if (!bRet)
						goto Exit0;
				}
			}
			else
			{
				bRet = pfnCallBack(EnumFileType_File, szCurrentPath, fd.name, &fd, user_data);
				if (!bRet)
					goto Exit0;
			}

			if (_wfindnext(hFind, &fd) == -1)
				break;
		}

Exit0:
		if (hFind != -1)
			_findclose(hFind);
		BOOL bRet1 = pfnCallBack(EnumFileType_LeaveDirectory, szCurrentPath, NULL, NULL, user_data);
		return (bRet && bRet1);
	}

	//////////////////////////////////////////////////////////////////////////
	// 保证目录存在
 	inline LPSECURITY_ATTRIBUTES InitSecurityAttributes(SECURITY_ATTRIBUTES &sa)
 	{
 		sa.bInheritHandle	= TRUE;
 		sa.nLength			= sizeof(sa);
 		LPCWSTR szSD		= 
 			L"D:"						// Discretionary ACL
 			L"(A;OICI;GRGWGXSD;;;AU)"	// Allow Modify to Authenticated users
 			L"(A;OICI;GA;;;SY)"			// Allow full control to system
 			L"(A;OICI;GA;;;BA)";		// Allow full control to administrators
 
 		BOOL bRet = ::ConvertStringSecurityDescriptorToSecurityDescriptor(szSD, SDDL_REVISION_1, &sa.lpSecurityDescriptor, NULL);
 		if (bRet)
 			return &sa;
 		else
 			return NULL;
 	}

	inline HRESULT CreateDir(LPCWSTR szPath)
	{
		HRESULT hr = S_OK;
		{
			CPath path = szPath;
			path.AddBackslash();
			if (path.IsRoot())
				goto Exit0;
			path.RemoveBackslash();

			// 有可能出现文件名就是要创建的文件夹名称
			if (!path.IsDirectory() && path.FileExists())
				::DeleteFile(path);

			SECURITY_ATTRIBUTES sa = { 0 };
			LONG errorCode = ::SHCreateDirectoryEx(NULL, path, FileHlp::InitSecurityAttributes(sa));
			ERROR_CHECK_BOOLEX_ERR3(path.IsDirectory(), hr = E_FAIL, L"szFilePath = %s, errorcode = %d", szPath, errorCode);	// 这里有可能失败，加入详细错误码 chuanjie
		}

Exit0:
		return hr;
	}

	inline HRESULT DeleteFileToRecycleBinForUI(LPCWSTR szFilePath)
	{// 用于手动执行
		HRESULT hr = S_OK;
		{
			::SetFileAttributes(szFilePath, FILE_ATTRIBUTE_NORMAL);

			WCHAR szPath[MAX_PATH + 1] = {0};
			wcscpy(szPath, szFilePath);
			ERROR_CHECK_BOOLEX_ERR2('\0' == szPath[wcslen(szPath) + 1], hr = E_FAIL, L"szFilePath = %s ", szPath);
			// Shell Api要求路径结尾有两个'\0'

			SHFILEOPSTRUCTW SFOP;
			SFOP.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
			SFOP.pFrom	= szPath;
			SFOP.pTo	= NULL;
			SFOP.wFunc	= FO_DELETE;

			int nRet = ::SHFileOperationW(&SFOP);
			if (0 != nRet)
				hr = E_FAIL;
		}
Exit0:
		return hr;
	}

	inline HRESULT DeleteFileToRecycleBin(LPCWSTR szFilePath)
	{
		HRESULT hr = S_OK;
		{
			::SetFileAttributes(szFilePath, FILE_ATTRIBUTE_NORMAL);

			WCHAR szPath[MAX_PATH + 1] = {0};
			wcscpy(szPath, szFilePath);
			ASSERT('\0' == szPath[wcslen(szPath) + 1]); // Shell Api要求路径结尾有两个'\0'

			SHFILEOPSTRUCTW SFOP;
			SFOP.fFlags = FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
			SFOP.pFrom	= szPath;
			SFOP.pTo	= NULL;
			SFOP.wFunc	= FO_DELETE;

			int nRet = ::SHFileOperationW(&SFOP);
			if (0 != nRet)
				hr = E_FAIL;
		}
		//Exit0:
		return hr;
	}

	// 文件从某些系统目录拷出时，目标文件会被去掉某个用户的写入权限，此函数保证目标文件可写
	inline HRESULT SafeCopyFile(LPCWSTR szSrc, LPCWSTR szDest)
	{
		HRESULT hr = S_OK;
		{
			const int _BUF_SIZE = 64 * 1024;

			CHandle hSrc(::CreateFileW(szSrc, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
			ERROR_CHECK_BOOLEX(hSrc != INVALID_HANDLE_VALUE, hr = E_FAIL);
			SECURITY_ATTRIBUTES sa = { 0 };
			CHandle hDest(::CreateFileW(szDest, GENERIC_WRITE, 0, InitSecurityAttributes(sa), CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL));
			ERROR_CHECK_BOOLEX(hDest != INVALID_HANDLE_VALUE, hr = E_FAIL);

			CTempBuffer<BYTE> buf(_BUF_SIZE);
			DWORD dwReadSize = 0;
			do 
			{
				ZeroMemory(buf, _BUF_SIZE);
				BOOL bRet = ::ReadFile(hSrc, buf, _BUF_SIZE, &dwReadSize, NULL);
				ERROR_CHECK_BOOLEX(bRet, hr = E_FAIL);
				if (dwReadSize)
				{
					DWORD dwWriteSize = 0;
					bRet = ::WriteFile(hDest, buf, dwReadSize, &dwWriteSize, NULL);
					ERROR_CHECK_BOOLEX(bRet, hr = E_FAIL);
				}
			} 
			while (_BUF_SIZE == dwReadSize);
		}
Exit0:
		return hr;
	}

	inline HRESULT CopyDirectories(LPCWSTR szSrcPath, LPCWSTR szDestPath, std::vector<CString> vecFolders,  BOOL* pbCancel)
	{
		CPath pathRoot = szSrcPath;
		ASSERT(pathRoot.IsDirectory());

		std::queue<CPath> quTreeNodes;
		quTreeNodes.push(pathRoot);

		std::queue<CPath> srcPathGuard;
		srcPathGuard.push(szSrcPath);

		std::queue<CPath> dstPathGuard;
		dstPathGuard.push(szDestPath);

		std::vector<CString>::iterator it = vecFolders.begin();

		BOOL bRet = TRUE;
		{
			while (srcPathGuard.size())
			{
				if (*pbCancel)
				{
					break;
				}

				CPath srcPath = srcPathGuard.front();
				srcPathGuard.pop();
				ASSERT(srcPath.IsDirectory());

				CPath dstPath = dstPathGuard.front();
				dstPathGuard.pop();

				CPath pathFindMask = srcPath;
				pathFindMask.Append(L"*.*");

				_wfinddata_t	fd		= {0};
				intptr_t		hFind	= _wfindfirst(pathFindMask, &fd);
				int				nRet	= 0;
				while(-1 != hFind && -1 != nRet)
				{
					if (*pbCancel)
					{
						break;
					}

					if (fd.attrib & _A_SUBDIR)
					{
						CString strName(fd.name);
						if (strName != L"." && strName != L"..")
						{
							BOOL bExclude = FALSE;
							for (it = vecFolders.begin(); it != vecFolders.end(); ++it)
							{	
								CString strNameTemp = L"/"; 
								strNameTemp += strName;
								if (strNameTemp.CompareNoCase(*it) == 0 || *pbCancel)
								{
									bExclude = TRUE;
									break;
								}
							}

							if (*pbCancel)
								break;

							if (!bExclude && strName.CompareNoCase(L"Tencent Files") != 0)
							{	// 过虑文件夹
								CPath srcPathSubNode = srcPath;
								srcPathSubNode.Append(fd.name);
								srcPathSubNode.AddBackslash();
								srcPathGuard.push(srcPathSubNode);

								CPath dstPathSubNode = dstPath;
								dstPathSubNode.Append(fd.name);
								dstPathSubNode.AddBackslash();
								dstPathGuard.push(dstPathSubNode);
							}
						}			
					}
					else if (!(fd.attrib & _A_HIDDEN) && !(fd.attrib & _A_SYSTEM))	// 只复制正常文件
					{
						CPath srcFile(srcPath);
						srcFile.Append(fd.name);
						if (fd.size <= 300 * 1024 * 1024)	// 小于300MB
						{
							CString strExten(srcFile.GetExtension());

							BOOL bExclude = FALSE;
							for (it = vecFolders.begin(); it != vecFolders.end(); ++it)
							{
								if (strExten.CompareNoCase(*it) == 0 || *pbCancel)
								{
									bExclude = TRUE;
									break;
								}
							}

							if (*pbCancel)
								break;

							if (!bExclude)//(strExten.CompareNoCase(L".lnk") != 0) // 不拷贝快捷方式
							{	
								CPath dstFile(dstPath);
								if (!dstPath.IsDirectory())
									CreateDir(dstPath.m_strPath);

								dstFile.Append(fd.name);
								::CopyFile(srcFile.m_strPath, dstFile.m_strPath, FALSE);
							}
						}

					}
					nRet = _wfindnext(hFind, &fd);
				}

				_findclose(hFind);
				CHECK_BOOL(bRet);
			}
		}

Exit0:
		return bRet;
	}

	/////////////////////////////////////////////////////////////////////////
	// 删除整个目录，不管是否为空
BOOL inline __stdcall __ENUM_FILE2_DeleteDirectories(
		EnumFileType	eEnumFileType, 
		LPCWSTR			szPath, 
		LPCWSTR			szFile, 
		_wfinddata_t	*pfd, 
		LPVOID			user_data)
	{
		ASSERT(user_data);
		HRESULT hr = S_OK;
		BOOL	bRet = TRUE;
		BOOL bToRecycleBin = ((_PAIR_DELETE_*)user_data)->first;
		BOOL bIfAsPossible = ((_PAIR_DELETE_*)user_data)->second;

		if (eEnumFileType == EnumFileType_File)
		{
			WCHAR szFilePath[MAX_PATH];
			wcscpy(szFilePath, szPath);
			wcscat(szFilePath, szFile);

			if (bToRecycleBin)
			{
				hr = DeleteFileToRecycleBin(szFilePath);
				ERROR_CHECK_COM(hr);
			}
			else
			{
				::SetFileAttributes(szFilePath, FILE_ATTRIBUTE_NORMAL);
				bRet = ::DeleteFileW(szFilePath);
				ERROR_CHECK_BOOL(bRet);
			}
		}
		else if (eEnumFileType == EnumFileType_LeaveDirectory)
		{
			::RemoveDirectoryW(szPath);
			CPath path(szPath);
			ERROR_CHECK_BOOLEX(!path.IsDirectory(), hr = E_FAIL);
		}

Exit0:
		if (bIfAsPossible)
			return TRUE;
		else
			return (bRet && hr == S_OK);
	}

	inline BOOL DeleteDirectories(LPCWSTR szPath, BOOL bToRecycleBin = FALSE, BOOL bIfAsPossible = FALSE)
	{
		_PAIR_DELETE_ data;
		data.first = bToRecycleBin;
		data.second = bIfAsPossible;
		return EnumFile2(szPath, L"*.*", __ENUM_FILE2_DeleteDirectories, &data);
	} 

	inline void GetLongPath(LPCWSTR szSourcePath, CString& strLongPath)
	{
		if (wcslen(szSourcePath) >= MAX_PATH)
			strLongPath.Append(L"\\\\?\\");
		strLongPath.Append(szSourcePath);
	} 

	inline HRESULT GetFileSize(LPCWSTR szFilePath, LONGLONG &lSize)
	{
		ASSERT(!IsStrEmpty(szFilePath));
		HRESULT hr = S_OK;
		{
			CString strLongPath;
			GetLongPath(szFilePath,strLongPath);
			WIN32_FILE_ATTRIBUTE_DATA WFAD;
			BOOL bRet = ::GetFileAttributesExW(strLongPath, GetFileExInfoStandard, &WFAD);
			ERROR_CHECK_BOOLEX(bRet, hr = E_FAIL);
			lSize = WFAD.nFileSizeHigh;
			lSize = lSize << sizeof(WFAD.nFileSizeLow) * 8;
			lSize += WFAD.nFileSizeLow;
		}
Exit0:
		return hr;
	}

	inline HRESULT GetFileTime(LPCWSTR szFilePath, CString &strMTime)
	{
		ASSERT(!IsStrEmpty(szFilePath));
		ASSERT(IsStrEmpty(strMTime));

		HRESULT hr = S_OK;
		{
			CString strLongPath;
			GetLongPath(szFilePath,strLongPath);

			WIN32_FILE_ATTRIBUTE_DATA WFAD;
			BOOL bRet = ::GetFileAttributesExW(strLongPath, GetFileExInfoStandard, &WFAD);
			ERROR_CHECK_BOOLEX_ERR(bRet, hr = E_FAIL, _CTraceStack::FormatArgs(L"Can Not Get File Time: %s lasterror = %d", szFilePath, ::GetLastError()));

			FILETIME ftModified = {0};
			::FileTimeToLocalFileTime(&WFAD.ftLastWriteTime, &ftModified);
			SYSTEMTIME stModified	= {0};
			::FileTimeToSystemTime(&ftModified, &stModified);

			__int64 &liMTimeNanoSec = (__int64 &)WFAD.ftLastWriteTime;
			strMTime.Format(
				L"%04d-%02d-%02d %02d:%02d:%02d", 
				stModified.wYear, 
				stModified.wMonth, 
				stModified.wDay, 
				stModified.wHour, 
				stModified.wMinute, 
				stModified.wSecond
				);
		}
Exit0:
		return hr;
	}

	inline HRESULT GetFimeTimeT(LPCWSTR szFilePath, FILETIME& ft)
	{
		HRESULT hr = S_OK;
		{
			CString strLongPath;
			GetLongPath(szFilePath,strLongPath);

			WIN32_FILE_ATTRIBUTE_DATA WFAD;
			BOOL bRet = ::GetFileAttributesExW(strLongPath, GetFileExInfoStandard, &WFAD);
			ERROR_CHECK_BOOLEX_ERR(bRet, hr = E_FAIL, _CTraceStack::FormatArgs(L"Can Not Get File Time: %s lasterror = %d", szFilePath, ::GetLastError()));
			ft = WFAD.ftLastWriteTime;
			// ::FileTimeToLocalFileTime(&WFAD.ftLastWriteTime, &ft);
		}
Exit0:
		return hr;
	}

    inline FILETIME TimetToFileTime(time_t t)
    {
        LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;

        FILETIME ft = {0};
        ft.dwLowDateTime = (DWORD) ll;
        ft.dwHighDateTime = ll >> 32;

        return ft;
    }

    inline HRESULT SetFileCreateTime(LPCWSTR szFilePath, time_t time)
    {
        HRESULT hr = S_OK;
        {
            CHandle hFile(::CreateFile(
                szFilePath, 
                FILE_WRITE_ATTRIBUTES, 
                FILE_SHARE_WRITE | FILE_SHARE_READ ,
                NULL, 
                OPEN_EXISTING, 
                FILE_ATTRIBUTE_NORMAL,
                NULL));

            FILETIME ft = TimetToFileTime(time);

            BOOL bRet = ::SetFileTime(hFile, &ft, NULL, NULL);
            ERROR_CHECK_BOOLEX(bRet, hr = E_FAIL);
        }

    Exit0:
        return hr;
    }

    inline HRESULT SetFileModifyTime(LPCWSTR szFilePath, time_t time)
    {
        HRESULT hr = S_OK;
        {
            CHandle hFile(::CreateFile(
                szFilePath, 
                FILE_WRITE_ATTRIBUTES, 
                FILE_SHARE_WRITE | FILE_SHARE_READ ,
                NULL, 
                OPEN_EXISTING, 
                FILE_ATTRIBUTE_NORMAL,
                NULL));

            FILETIME ft = TimetToFileTime(time);

            BOOL bRet = ::SetFileTime(hFile, NULL, NULL, &ft);
            ERROR_CHECK_BOOLEX(bRet, hr = E_FAIL);
        }

    Exit0:
        return hr;
    }

	/*
	CString filedesc[] = {L"FILE_READ_DATA", L"FILE_WRITE_DATA", L"FILE_APPEND_DATA", L"FILE_READ_EA",
	L"FILE_WRITE_EA", L"FILE_EXECUTE", L"FILE_DELETE_CHILD", L"FILE_READ_ATTRIBUTES",
	L"FILE_WRITE_ATTRIBUTES", L" ", L" ", L" ",
	L" ", L" ", L" ", L" ",
	L"DELETE ", L"READ_CONTROL", L"WRITE_DAC", L"WRITE_OWNER",
	L"SYNCHRONIZE ", L" ", L" ",L" ",
	L"ACCESS_SYSTEM_SECURITY", L"MAXIMUM_ALLOWED", L" ",L" ",
	L"GENERIC_ALL", L"GENERIC_EXECUTE", L"GENERIC_WRITE",L"GENERIC_READ"};

	CString rights[] = {L"Allow", L"Deny"};
	*/
	inline BOOL GetAccountRights(LPCWSTR lpszFileName, int (&arrRights)[32]) 
	{
		//将参数arrRights初始化为0
		for (int i = 0; i < 32; i++)
		{
			arrRights[i] = 0;
		}
		// 声明和LookupAccountName相关的变量（注意，全为0，要在程序中动态分配）
		DWORD          cbUserSID      = 0;
		// 和文件相关的安全描述符 SD 的变量
		PSECURITY_DESCRIPTOR pFileSD  = NULL;     // 结构变量
		DWORD          cbFileSD       = 0;        // SD的size
		// 和ACL 相关的变量
		PACL           pACL           = NULL;
		BOOL           fDaclPresent;
		BOOL           fDaclDefaulted;
		ACL_SIZE_INFORMATION AclInfo;
		// 一个临时使用的 ACE 变量
		LPVOID         pTempAce       = NULL;
		UINT           CurrentAceIndex = 0;  //ACE在ACL中的位置
		//API函数的返回值，假设所有的函数都返回失败。
		BOOL           fResult = FALSE;
		BOOL           fAPISuccess = FALSE;
		SECURITY_INFORMATION secInfo = DACL_SECURITY_INFORMATION;

		HANDLE hAccessToken;  
		BYTE * InfoBuffer = new BYTE[1024];  
		DWORD dwInfoBufferSize;  
		PSID pUsersid = NULL;  
		SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;  

		__try {

			// STEP 1: 取得当前用户的SID
			if(!::OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hAccessToken))  
			{  
				return FALSE;  
			}  

			if(!::GetTokenInformation(hAccessToken,TokenGroups,InfoBuffer,1024,&dwInfoBufferSize))  
			{  
				::CloseHandle(hAccessToken);  
				return FALSE;  
			}  
			::CloseHandle(hAccessToken);  

			if(!::AllocateAndInitializeSid(&siaNtAuthority,  
				2,  
				SECURITY_BUILTIN_DOMAIN_RID,  
				DOMAIN_ALIAS_RID_ADMINS,  
				0,0,0,0,0,0,  
				&pUsersid))  
			{  
				return FALSE;  
			}  

			//
			// 
			// STEP 2: 取得文件（目录）相关的安全描述符SD
			//     使用GetFileSecurity函数取得一份文件SD的拷贝，同样，这个函数也
			// 是被调用两次，第一次同样是取SD的内存长度。注意，SD有两种格式：自相关的
			// （self-relative）和 完全的（absolute），GetFileSecurity只能取到“自
			// 相关的”，而SetFileSecurity则需要完全的。这就是为什么需要一个新的SD，
			// 而不是直接在GetFileSecurity返回的SD上进行修改。因为“自相关的”信息
			// 是不完整的。
			fAPISuccess = GetFileSecurity(lpszFileName, 
				secInfo, pFileSD, 0, &cbFileSD);
			// 以上调用API会失败，失败原因是内存不足。并把所需要的内存大小传出。
			// 下面是处理非内存不足的错误。
			if (fAPISuccess)
				__leave;
			else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
				_tprintf(TEXT("GetFileSecurity() failed. Error %d\n"), 
					GetLastError());
				__leave;
			}
			pFileSD = ::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbFileSD) ;
			if (!pFileSD) {
				_tprintf(TEXT("HeapAlloc() failed. Error %d\n"), GetLastError());
				__leave;
			}
			fAPISuccess = GetFileSecurity(lpszFileName, 
				secInfo, pFileSD, cbFileSD, &cbFileSD);
			if (!fAPISuccess) {
				_tprintf(TEXT("GetFileSecurity() failed. Error %d\n"), 
					GetLastError());
				__leave;
			}
			// 
			// STEP 3: 从GetFileSecurity 返回的SD中取DACL
			// 
			if (!GetSecurityDescriptorDacl(pFileSD, &fDaclPresent, &pACL,
				&fDaclDefaulted)) {
					_tprintf(TEXT("GetSecurityDescriptorDacl() failed. Error %d\n"),
						GetLastError());
					__leave;
			}
			// 
			// STEP 4: 取 DACL的内存size
			//     GetAclInformation可以提供DACL的内存大小。只传入一个类型为
			// ACL_SIZE_INFORMATION的structure的参数，需DACL的信息，是为了
			// 方便我们遍历其中的ACE。
			AclInfo.AceCount = 0; // Assume NULL DACL.
			AclInfo.AclBytesFree = 0;
			AclInfo.AclBytesInUse = sizeof(ACL);
			if (pACL == NULL)
				fDaclPresent = FALSE;
			// 如果DACL不为空，则取其信息。（大多数情况下“自关联”的DACL为空）
			if (fDaclPresent) {            
				if (!GetAclInformation(pACL, &AclInfo, 
					sizeof(ACL_SIZE_INFORMATION), AclSizeInformation)) {
						_tprintf(TEXT("GetAclInformation() failed. Error %d\n"),
							GetLastError());
						__leave;
				}
			}
			// 
			// STEP 5  如果文件（目录） DACL 有数据，将指定帐户的ACE的访问权限转换到整型数组
			// 
			//     下面的代码假设首先检查指定文件（目录）是否存在的DACL，如果有的话，
			// 那么就将指定帐户的ACE的访问权限转换到整型数组，我们可以看到其遍历的方法
			// 是采用ACL_SIZE_INFORMATION结构中的AceCount成员来完成的。在这个循环中，
			// 查找和指定账户相关的ACE
			// 
			if (fDaclPresent && AclInfo.AceCount) {
				for (CurrentAceIndex = 0; 
					CurrentAceIndex < AclInfo.AceCount;
					CurrentAceIndex++) {
						// 
						// STEP 10: 从DACL中取ACE
						// 
						if (!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
							_tprintf(TEXT("GetAce() failed. Error %d\n"), 
								GetLastError());
							__leave;
						}
						// 
						// 
						// STEP 6: 检查要拷贝的ACE的SID是否和需要加入的ACE的SID一样，
						// 如果一样，那么就将该ACE的访问权限转换到整型数组，
						// 否则跳过，进行下一个循环
						// 
						int nAceType = 1;
						if (EqualSid(pUsersid,&(((ACCESS_ALLOWED_ACE *)pTempAce)->SidStart)))
						{
							if(((PACE_HEADER)pTempAce)->AceType == ACCESS_DENIED_ACE_TYPE)
							{
								nAceType = 2;
							}
							else
							{
								nAceType = 1;
							}
							//bitset类代表的整型数值的顺序是从0到N-1
							std::bitset<32> bitAccessMask((int)((ACCESS_ALLOWED_ACE*)pTempAce)->Mask);
							for (int i = 0; i < 32; i++)
							{
								if (bitAccessMask[i] != 0 && arrRights[i] != 2)
								{
									arrRights[i] = nAceType;
								}
							}
						}
						else
						{
							continue;
						}
				}
			}
			fResult = TRUE;
		} __finally {
			// 
			// STEP 7: 释放已分配的内存，以免Memory Leak
			//
			delete InfoBuffer;  

			if (pFileSD)
				(HeapFree(GetProcessHeap(), 0, pFileSD));
		}
		return fResult;
	}


	typedef BOOL (WINAPI *SetSecurityDescriptorControlFnPtr)(
		IN PSECURITY_DESCRIPTOR pSecurityDescriptor,
		IN SECURITY_DESCRIPTOR_CONTROL ControlBitsOfInterest,
		IN SECURITY_DESCRIPTOR_CONTROL ControlBitsToSet);
	typedef BOOL (WINAPI *AddAccessAllowedAceExFnPtr)(
		PACL pAcl,
		DWORD dwAceRevision,
		DWORD AceFlags,
		DWORD AccessMask,
		PSID pSid
		);
	// 增加权限
	inline BOOL AddAccountRights(LPCWSTR lpszFileName, DWORD dwAccessMask) 
	{

		// 声明SID变量
		//SID_NAME_USE   snuType;
		// 声明和LookupAccountName相关的变量（注意，全为0，要在程序中动态分配）
		// 	LPVOID         pUserSID       = NULL;
		// 	DWORD          cbUserSID      = 0;
		// 和文件相关的安全描述符 SD 的变量
		PSECURITY_DESCRIPTOR pFileSD  = NULL;     // 结构变量
		DWORD          cbFileSD       = 0;        // SD的size
		// 一个新的SD的变量，用于构造新的ACL（把已有的ACL和需要新加的ACL整合起来）
		SECURITY_DESCRIPTOR  newSD;
		// 和ACL 相关的变量
		PACL           pACL           = NULL;
		BOOL           fDaclPresent;
		BOOL           fDaclDefaulted;
		ACL_SIZE_INFORMATION AclInfo;
		// 一个新的 ACL 变量
		PACL           pNewACL        = NULL;  //结构指针变量
		DWORD          cbNewACL       = 0;     //ACL的size
		// 一个临时使用的 ACE 变量
		LPVOID         pTempAce       = NULL;
		UINT           CurrentAceIndex = 0;  //ACE在ACL中的位置
		UINT           newAceIndex = 0;  //新添的ACE在ACL中的位置
		//API函数的返回值，假设所有的函数都返回失败。
		BOOL           fResult = FALSE;
		BOOL           fAPISuccess = FALSE;
		SECURITY_INFORMATION secInfo = DACL_SECURITY_INFORMATION;

		HANDLE hAccessToken;  
		BYTE * InfoBuffer = new BYTE[1024];  
		DWORD dwInfoBufferSize;  
		PSID pUsersid = NULL;  
		SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;  


		// 下面的两个函数是新的API函数，仅在Windows 2000以上版本的操作系统支持。 
		// 在此将从Advapi32.dll文件中动态载入。如果你使用VC++ 6.0编译程序，而且你想
		// 使用这两个函数的静态链接。则请为你的编译加上：/D_WIN32_WINNT=0x0500
		// 的编译参数。并且确保你的SDK的头文件和lib文件是最新的。
		SetSecurityDescriptorControlFnPtr _SetSecurityDescriptorControl = NULL;
		AddAccessAllowedAceExFnPtr _AddAccessAllowedAceEx = NULL; 
		__try {
			// 
			// STEP 1: 通过用户名取得SID
			//     在这一步中LookupAccountName函数被调用了两次，第一次是取出所需要
			// 的内存的大小，然后，进行内存分配。第二次调用才是取得了用户的帐户信息。
			// LookupAccountName同样可以取得域用户或是用户组的信息。（请参看MSDN）
			//

			// STEP 1: 取得当前用户的SID
			if(!::OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hAccessToken))  
			{  
				return FALSE;  
			}  

			if(!::GetTokenInformation(hAccessToken,TokenGroups,InfoBuffer,1024,&dwInfoBufferSize))  
			{  
				::CloseHandle(hAccessToken);  
				return FALSE;  
			}  
			::CloseHandle(hAccessToken);  

			if(!::AllocateAndInitializeSid(&siaNtAuthority,  
				2,  
				SECURITY_BUILTIN_DOMAIN_RID,  
				DOMAIN_ALIAS_RID_ADMINS,  
				0,0,0,0,0,0,  
				&pUsersid))  
			{  
				return FALSE;  
			}  

			// 
			// STEP 2: 取得文件（目录）相关的安全描述符SD
			//     使用GetFileSecurity函数取得一份文件SD的拷贝，同样，这个函数也
			// 是被调用两次，第一次同样是取SD的内存长度。注意，SD有两种格式：自相关的
			// （self-relative）和 完全的（absolute），GetFileSecurity只能取到“自
			// 相关的”，而SetFileSecurity则需要完全的。这就是为什么需要一个新的SD，
			// 而不是直接在GetFileSecurity返回的SD上进行修改。因为“自相关的”信息
			// 是不完整的。
			fAPISuccess = GetFileSecurity(lpszFileName,	secInfo, pFileSD, 0, &cbFileSD);
			// 以上调用API会失败，失败原因是内存不足。并把所需要的内存大小传出。
			// 下面是处理非内存不足的错误。
			if (fAPISuccess)
				__leave;
			else if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
				_tprintf(TEXT("GetFileSecurity() failed. Error %d\n"), 
					GetLastError());
				__leave;
			}
			pFileSD = ::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbFileSD);
			if (!pFileSD) {
				_tprintf(TEXT("HeapAlloc() failed. Error %d\n"), GetLastError());
				__leave;
			}
			fAPISuccess = GetFileSecurity(lpszFileName, 
				secInfo, pFileSD, cbFileSD, &cbFileSD);
			if (!fAPISuccess) {
				_tprintf(TEXT("GetFileSecurity() failed. Error %d\n"), 
					GetLastError());
				__leave;
			}
			// 
			// STEP 3: 初始化一个新的SD
			// 
			if (!InitializeSecurityDescriptor(&newSD, 
				SECURITY_DESCRIPTOR_REVISION)) {
					_tprintf(TEXT("InitializeSecurityDescriptor() failed.")
						TEXT("Error %d\n"), GetLastError());
					__leave;
			}
			// 
			// STEP 4: 从GetFileSecurity 返回的SD中取DACL
			// 
			if (!GetSecurityDescriptorDacl(pFileSD, &fDaclPresent, &pACL,
				&fDaclDefaulted)) {
					_tprintf(TEXT("GetSecurityDescriptorDacl() failed. Error %d\n"),
						GetLastError());
					__leave;
			}
			// 
			// STEP 5: 取 DACL的内存size
			//     GetAclInformation可以提供DACL的内存大小。只传入一个类型为
			// ACL_SIZE_INFORMATION的structure的参数，需DACL的信息，是为了
			// 方便我们遍历其中的ACE。
			AclInfo.AceCount = 0; // Assume NULL DACL.
			AclInfo.AclBytesFree = 0;
			AclInfo.AclBytesInUse = sizeof(ACL);
			if (pACL == NULL)
				fDaclPresent = FALSE;
			// 如果DACL不为空，则取其信息。（大多数情况下“自关联”的DACL为空）
			if (fDaclPresent) {            
				if (!GetAclInformation(pACL, &AclInfo, 
					sizeof(ACL_SIZE_INFORMATION), AclSizeInformation)) {
						_tprintf(TEXT("GetAclInformation() failed. Error %d\n"),
							GetLastError());
						__leave;
				}
			}
			// 
			// STEP 6: 计算新的ACL的size
			//    计算的公式是：原有的DACL的size加上需要添加的一个ACE的size，以
			// 及加上一个和ACE相关的SID的size，最后减去两个字节以获得精确的大小。
			cbNewACL = AclInfo.AclBytesInUse + sizeof(ACCESS_ALLOWED_ACE) 
				+ GetLengthSid(pUsersid) - sizeof(DWORD);

			// 
			// STEP 7: 为新的ACL分配内存
			// 
			pNewACL = (PACL) (HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbNewACL));
			if (!pNewACL) {
				_tprintf(TEXT("HeapAlloc() failed. Error %d\n"), GetLastError());
				__leave;
			}
			// 
			// STEP 8: 初始化新的ACL结构
			// 
			if (!InitializeAcl(pNewACL, cbNewACL, ACL_REVISION2)) {
				_tprintf(TEXT("InitializeAcl() failed. Error %d\n"), 
					GetLastError());
				__leave;
			} 

			// 
			// STEP 9  如果文件（目录） DACL 有数据，拷贝其中的ACE到新的DACL中
			// 
			//     下面的代码假设首先检查指定文件（目录）是否存在的DACL，如果有的话，
			// 那么就拷贝所有的ACE到新的DACL结构中，我们可以看到其遍历的方法是采用
			// ACL_SIZE_INFORMATION结构中的AceCount成员来完成的。在这个循环中，
			// 会按照默认的ACE的顺序来进行拷贝（ACE在ACL中的顺序是很关键的），在拷
			// 贝过程中，先拷贝非继承的ACE（我们知道ACE会从上层目录中继承下来）
			// 
			newAceIndex = 0;
			if (fDaclPresent && AclInfo.AceCount) {
				for (CurrentAceIndex = 0; 
					CurrentAceIndex < AclInfo.AceCount;
					CurrentAceIndex++) {
						// 
						// STEP 10: 从DACL中取ACE
						// 
						if (!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
							_tprintf(TEXT("GetAce() failed. Error %d\n"), 
								GetLastError());
							__leave;
						}
						// 
						// STEP 11: 检查是否是非继承的ACE
						//     如果当前的ACE是一个从父目录继承来的ACE，那么就退出循环。
						// 因为，继承的ACE总是在非继承的ACE之后，而我们所要添加的ACE
						// 应该在已有的非继承的ACE之后，所有的继承的ACE之前。退出循环
						// 正是为了要添加一个新的ACE到新的DACL中，这后，我们再把继承的
						// ACE拷贝到新的DACL中。
						//
						if (((ACCESS_ALLOWED_ACE *)pTempAce)->Header.AceFlags
							& INHERITED_ACE)
							break;
						// 
						// STEP 12: 检查要拷贝的ACE的SID是否和需要加入的ACE的SID一样，
						// 如果一样，那么就应该废掉已存在的ACE，也就是说，同一个用户的存取
						// 权限的设置的ACE，在DACL中应该唯一。这在里，跳过对同一用户已设置
						// 了的ACE，仅是拷贝其它用户的ACE。
						// 
						if (EqualSid(pUsersid,
							&(((ACCESS_ALLOWED_ACE *)pTempAce)->SidStart)))
						{
							ACCESS_ALLOWED_ACE pTempAce2 = *(ACCESS_ALLOWED_ACE *)pTempAce;

							ACCESS_DENIED_ACE pTempAce4 = *(ACCESS_DENIED_ACE *)pTempAce;
							int a = -1;
							if (pTempAce2.Header.AceType == ACCESS_ALLOWED_ACE_TYPE)
							{
								a = 0;
							}
							else if (pTempAce2.Header.AceType == ACCESS_DENIED_ACE_TYPE)
							{
								a = 1;
							}
							else
								a = 2;
							continue;
						}
						// 
						// STEP 13: 把ACE加入到新的DACL中
						//    下面的代码中，注意 AddAce 函数的第三个参数，这个参数的意思是 
						// ACL中的索引值，意为要把ACE加到某索引位置之后，参数MAXDWORD的
						// 意思是确保当前的ACE是被加入到最后的位置。
						//
						if (!AddAce(pNewACL, ACL_REVISION, MAXDWORD, pTempAce,
							((PACE_HEADER) pTempAce)->AceSize)) {
								_tprintf(TEXT("AddAce() failed. Error %d\n"), 
									GetLastError());
								__leave;
						}
						newAceIndex++;
				}
			}

			// 
			// STEP 14: 把一个 access-allowed 的ACE 加入到新的DACL中
			//     前面的循环拷贝了所有的非继承且SID为其它用户的ACE，退出循环的第一件事
			// 就是加入我们指定的ACE。请注意首先先动态装载了一个AddAccessAllowedAceEx
			// 的API函数，如果装载不成功，就调用AddAccessAllowedAce函数。前一个函数仅
			// 在Windows 2000以后的版本支持，NT则没有，我们为了使用新版本的函数，我们首
			// 先先检查一下当前系统中可不可以装载这个函数，如果可以则就使用。使用动态链接
			// 比使用静态链接的好处是，程序运行时不会因为没有这个API函数而报错。
			// 
			// Ex版的函数多出了一个参数AceFlag（第三人参数），用这个参数我们可以来设置一
			// 个叫ACE_HEADER的结构，以便让我们所设置的ACE可以被其子目录所继承下去，而 
			// AddAccessAllowedAce函数不能定制这个参数，在AddAccessAllowedAce函数
			// 中，其会把ACE_HEADER这个结构设置成非继承的。
			// 
			_AddAccessAllowedAceEx = (AddAccessAllowedAceExFnPtr)
				GetProcAddress(GetModuleHandle(TEXT("advapi32.dll")),
				"AddAccessAllowedAceEx");
			if (_AddAccessAllowedAceEx) {
				if (!_AddAccessAllowedAceEx(pNewACL, ACL_REVISION2,
					CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE ,
					dwAccessMask, pUsersid)) {
						_tprintf(TEXT("AddAccessAllowedAceEx() failed. Error %d\n"),
							GetLastError());
						__leave;
				}
			}else{
				if (!AddAccessAllowedAce(pNewACL, ACL_REVISION2, 
					dwAccessMask, pUsersid)) {
						_tprintf(TEXT("AddAccessAllowedAce() failed. Error %d\n"),
							GetLastError());
						__leave;
				}
			}
			// 
			// STEP 15: 按照已存在的ACE的顺序拷贝从父目录继承而来的ACE
			// 
			std::bitset<32> bit((int)dwAccessMask);
			if (fDaclPresent && AclInfo.AceCount) {
				for (; 
					CurrentAceIndex < AclInfo.AceCount;
					CurrentAceIndex++) {
						// 
						// STEP 16: 从文件（目录）的DACL中继续取ACE
						// 
						if (!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
							_tprintf(TEXT("GetAce() failed. Error %d\n"), 
								GetLastError());
							__leave;
						}
						// 
						// STEP 17: 把ACE加入到新的DACL中
						// 
						if (!AddAce(pNewACL, ACL_REVISION, MAXDWORD, pTempAce,
							((PACE_HEADER) pTempAce)->AceSize)) {
								_tprintf(TEXT("AddAce() failed. Error %d\n"), 
									GetLastError());
								__leave;
						}
				}
			}
			// 
			// STEP 18: 把新的ACL设置到新的SD中
			// 
			if (!SetSecurityDescriptorDacl(&newSD, TRUE, pNewACL, 
				FALSE)) {
					_tprintf(TEXT("SetSecurityDescriptorDacl() failed. Error %d\n"),
						GetLastError());
					__leave;
			}
			// 
			// STEP 19: 把老的SD中的控制标记再拷贝到新的SD中，我们使用的是一个叫 
			// SetSecurityDescriptorControl() 的API函数，这个函数同样只存在于
			// Windows 2000以后的版本中，所以我们还是要动态地把其从advapi32.dll 
			// 中载入，如果系统不支持这个函数，那就不拷贝老的SD的控制标记了。
			// 
			_SetSecurityDescriptorControl =(SetSecurityDescriptorControlFnPtr)
				GetProcAddress(GetModuleHandle(TEXT("advapi32.dll")),
				"SetSecurityDescriptorControl");
			if (_SetSecurityDescriptorControl) {
				SECURITY_DESCRIPTOR_CONTROL controlBitsOfInterest = 0;
				SECURITY_DESCRIPTOR_CONTROL controlBitsToSet = 0;
				SECURITY_DESCRIPTOR_CONTROL oldControlBits = 0;
				DWORD dwRevision = 0;
				if (!GetSecurityDescriptorControl(pFileSD, &oldControlBits,
					&dwRevision)) {
						_tprintf(TEXT("GetSecurityDescriptorControl() failed.")
							TEXT("Error %d\n"), GetLastError());
						__leave;
				}
				if (oldControlBits & SE_DACL_AUTO_INHERITED) {
					controlBitsOfInterest =
						SE_DACL_AUTO_INHERIT_REQ |
						SE_DACL_AUTO_INHERITED ;
					controlBitsToSet = controlBitsOfInterest;
				}
				else if (oldControlBits & SE_DACL_PROTECTED) {
					controlBitsOfInterest = SE_DACL_PROTECTED;
					controlBitsToSet = controlBitsOfInterest;
				}        
				if (controlBitsOfInterest) {
					if (!_SetSecurityDescriptorControl(&newSD,
						controlBitsOfInterest,
						controlBitsToSet)) {
							_tprintf(TEXT("SetSecurityDescriptorControl() failed.")
								TEXT("Error %d\n"), GetLastError());
							__leave;
					}
				}
			}
			// 
			// STEP 20: 把新的SD设置设置到文件的安全属性中（千山万水啊，终于到了）
			// 
			if (!SetFileSecurity(lpszFileName, secInfo,
				&newSD)) {
					_tprintf(TEXT("SetFileSecurity() failed. Error %d\n"), 
						GetLastError());
					__leave;
			}
			fResult = TRUE;
		} __finally {
			// 
			// STEP 21: 释放已分配的内存，以免Memory Leak
			// 
			if (pFileSD) (::HeapFree(GetProcessHeap(), 0, pFileSD));
			if (pNewACL) (::HeapFree(GetProcessHeap(), 0, pNewACL));
		}
		return fResult;

	}

	inline BOOL IsSamePath(LPCWSTR _szPath1, LPCWSTR _szPath2)
	{
		// 这个日志对效率有影响,太多循环要用了
		//	RECORD_TRACE_STACK(L"FileHlp::IsSamePath()", NULL, _CTraceStack::FormatArgs(L"_szPath1 = %s, _szPath2 = %s", _szPath1, _szPath2));
		LPCWSTR szPath1 = _szPath1, szPath2 = _szPath2;
		if (*szPath1 == '\\')
			++szPath1;
		if (*szPath2 == '\\')
			++szPath2;

		CPath path1(szPath1), path2(szPath2);
		path1.RemoveBackslash();
		path2.RemoveBackslash();
		return _wcsicmp(path1, path2) == 0;
	}

	inline HRESULT DeleteFile(LPCWSTR szFilePath)
	{
		HRESULT hr = S_OK;
		{
			CString strLongPath;
			GetLongPath(szFilePath,strLongPath);

			::SetFileAttributes(strLongPath, FILE_ATTRIBUTE_NORMAL);
			::DeleteFileW(strLongPath);
			ERROR_CHECK_BOOLEX(!::PathFileExistsW(strLongPath), hr = E_FAIL);
		}
Exit0:
		return hr;
	}

	inline DWORD64 GetFreespace2(CString sDrive)
	{
		DWORD64 qwFreeBytes;

		if (::GetDiskFreeSpaceEx(sDrive, (PULARGE_INTEGER)&qwFreeBytes, NULL, NULL))
		{
			return qwFreeBytes; 
		}
		return 0;
	}

	inline CPath GetParentPath(LPCWSTR szFullPath)
	{
		ASSERT(szFullPath);
		CPath path(szFullPath);
		path.RemoveBackslash();
		path.RemoveFileSpec();
		return path;
	}

	inline BOOL  CopyFileInc( const CString& strExistingFileName, CString& strNewFileName)
	{
		CPath filePath = strNewFileName;
		CPath pathTemp = strNewFileName;
		int index = 0;
		while (pathTemp.FileExists());
		{
			index++;
			CString strExt = filePath.GetExtension();
			CString strFileName = filePath.m_strPath;
			strFileName.Delete(strFileName.GetLength() - strExt.GetLength(), strExt.GetLength());
			CString strIndex;
			strIndex.Format(L"(%d)", index);
			strFileName.Append(strIndex);
			strFileName.Append(strExt);
			pathTemp.m_strPath = strFileName;			
		}

		if(CopyFile(strExistingFileName, pathTemp.m_strPath, FALSE))
		{
			strNewFileName = pathTemp.m_strPath;
			return TRUE;
		}
		return FALSE;
	}

}

// -------------------------------------------------------------------------
//	$Log: $

#endif /* __FILEHLPER_H__ */
