/* -------------------------------------------------------------------------
//	�ļ���		��	filehlper.h
//	������		��	����
//	����ʱ��	��	2008-11-6 19:42:36
//	��������	��	�ļ�/�ļ��и�����
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
	// ö���ļ�
	typedef enum { EnumFileType_File, EnumFileType_EnterDirectory, EnumFileType_LeaveDirectory } EnumFileType;
	typedef BOOL (__stdcall *PFN_ENUM_FILE)(LPCWSTR szPath, LPCWSTR szName, _wfinddata_t *pfd, LPVOID user_data);
	typedef BOOL (__stdcall *PFN_ENUM_FILE2)(EnumFileType eEnumFileType, LPCWSTR szPath, LPCWSTR szFile, _wfinddata_t *pfd, LPVOID user_data);

	typedef std::pair<BOOL, BOOL>				_PAIR_DELETE_;	// �Ƿ�ɾ��������վ���Ƿ����ɾ���еĴ���

	//////////////////////////////////////////////////////////////////////////
	// 
	inline BOOL _EnumFileBFS(LPCWSTR _szCurrentPath, PFN_ENUM_FILE pfnCallBack, LPVOID user_data, 
		BOOL bIncludeSubNode, LPCWSTR szMask, std::vector<CString>* pvecLongPaths)
	{
		CPath pathRoot = _szCurrentPath;
		ASSERT(pathRoot.IsDirectory());

		std::set<CNoCaseString> setFilters;	// ������ CNoCaseString �����Сд�޷�ƥ��
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
			//		path.RemoveBackslash();		// �����K:\����ȥ����������ֻ���Լ�ȥ
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
	// �������
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
	// ��֤Ŀ¼����
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

			// �п��ܳ����ļ�������Ҫ�������ļ�������
			if (!path.IsDirectory() && path.FileExists())
				::DeleteFile(path);

			SECURITY_ATTRIBUTES sa = { 0 };
			LONG errorCode = ::SHCreateDirectoryEx(NULL, path, FileHlp::InitSecurityAttributes(sa));
			ERROR_CHECK_BOOLEX_ERR3(path.IsDirectory(), hr = E_FAIL, L"szFilePath = %s, errorcode = %d", szPath, errorCode);	// �����п���ʧ�ܣ�������ϸ������ chuanjie
		}

Exit0:
		return hr;
	}

	inline HRESULT DeleteFileToRecycleBinForUI(LPCWSTR szFilePath)
	{// �����ֶ�ִ��
		HRESULT hr = S_OK;
		{
			::SetFileAttributes(szFilePath, FILE_ATTRIBUTE_NORMAL);

			WCHAR szPath[MAX_PATH + 1] = {0};
			wcscpy(szPath, szFilePath);
			ERROR_CHECK_BOOLEX_ERR2('\0' == szPath[wcslen(szPath) + 1], hr = E_FAIL, L"szFilePath = %s ", szPath);
			// Shell ApiҪ��·����β������'\0'

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
			ASSERT('\0' == szPath[wcslen(szPath) + 1]); // Shell ApiҪ��·����β������'\0'

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

	// �ļ���ĳЩϵͳĿ¼����ʱ��Ŀ���ļ��ᱻȥ��ĳ���û���д��Ȩ�ޣ��˺�����֤Ŀ���ļ���д
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
							{	// �����ļ���
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
					else if (!(fd.attrib & _A_HIDDEN) && !(fd.attrib & _A_SYSTEM))	// ֻ���������ļ�
					{
						CPath srcFile(srcPath);
						srcFile.Append(fd.name);
						if (fd.size <= 300 * 1024 * 1024)	// С��300MB
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

							if (!bExclude)//(strExten.CompareNoCase(L".lnk") != 0) // ��������ݷ�ʽ
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
	// ɾ������Ŀ¼�������Ƿ�Ϊ��
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
		//������arrRights��ʼ��Ϊ0
		for (int i = 0; i < 32; i++)
		{
			arrRights[i] = 0;
		}
		// ������LookupAccountName��صı�����ע�⣬ȫΪ0��Ҫ�ڳ����ж�̬���䣩
		DWORD          cbUserSID      = 0;
		// ���ļ���صİ�ȫ������ SD �ı���
		PSECURITY_DESCRIPTOR pFileSD  = NULL;     // �ṹ����
		DWORD          cbFileSD       = 0;        // SD��size
		// ��ACL ��صı���
		PACL           pACL           = NULL;
		BOOL           fDaclPresent;
		BOOL           fDaclDefaulted;
		ACL_SIZE_INFORMATION AclInfo;
		// һ����ʱʹ�õ� ACE ����
		LPVOID         pTempAce       = NULL;
		UINT           CurrentAceIndex = 0;  //ACE��ACL�е�λ��
		//API�����ķ���ֵ���������еĺ���������ʧ�ܡ�
		BOOL           fResult = FALSE;
		BOOL           fAPISuccess = FALSE;
		SECURITY_INFORMATION secInfo = DACL_SECURITY_INFORMATION;

		HANDLE hAccessToken;  
		BYTE * InfoBuffer = new BYTE[1024];  
		DWORD dwInfoBufferSize;  
		PSID pUsersid = NULL;  
		SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;  

		__try {

			// STEP 1: ȡ�õ�ǰ�û���SID
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
			// STEP 2: ȡ���ļ���Ŀ¼����صİ�ȫ������SD
			//     ʹ��GetFileSecurity����ȡ��һ���ļ�SD�Ŀ�����ͬ�����������Ҳ
			// �Ǳ��������Σ���һ��ͬ����ȡSD���ڴ泤�ȡ�ע�⣬SD�����ָ�ʽ������ص�
			// ��self-relative���� ��ȫ�ģ�absolute����GetFileSecurityֻ��ȡ������
			// ��صġ�����SetFileSecurity����Ҫ��ȫ�ġ������Ϊʲô��Ҫһ���µ�SD��
			// ������ֱ����GetFileSecurity���ص�SD�Ͻ����޸ġ���Ϊ������صġ���Ϣ
			// �ǲ������ġ�
			fAPISuccess = GetFileSecurity(lpszFileName, 
				secInfo, pFileSD, 0, &cbFileSD);
			// ���ϵ���API��ʧ�ܣ�ʧ��ԭ�����ڴ治�㡣��������Ҫ���ڴ��С������
			// �����Ǵ�����ڴ治��Ĵ���
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
			// STEP 3: ��GetFileSecurity ���ص�SD��ȡDACL
			// 
			if (!GetSecurityDescriptorDacl(pFileSD, &fDaclPresent, &pACL,
				&fDaclDefaulted)) {
					_tprintf(TEXT("GetSecurityDescriptorDacl() failed. Error %d\n"),
						GetLastError());
					__leave;
			}
			// 
			// STEP 4: ȡ DACL���ڴ�size
			//     GetAclInformation�����ṩDACL���ڴ��С��ֻ����һ������Ϊ
			// ACL_SIZE_INFORMATION��structure�Ĳ�������DACL����Ϣ����Ϊ��
			// �������Ǳ������е�ACE��
			AclInfo.AceCount = 0; // Assume NULL DACL.
			AclInfo.AclBytesFree = 0;
			AclInfo.AclBytesInUse = sizeof(ACL);
			if (pACL == NULL)
				fDaclPresent = FALSE;
			// ���DACL��Ϊ�գ���ȡ����Ϣ�������������¡��Թ�������DACLΪ�գ�
			if (fDaclPresent) {            
				if (!GetAclInformation(pACL, &AclInfo, 
					sizeof(ACL_SIZE_INFORMATION), AclSizeInformation)) {
						_tprintf(TEXT("GetAclInformation() failed. Error %d\n"),
							GetLastError());
						__leave;
				}
			}
			// 
			// STEP 5  ����ļ���Ŀ¼�� DACL �����ݣ���ָ���ʻ���ACE�ķ���Ȩ��ת������������
			// 
			//     ����Ĵ���������ȼ��ָ���ļ���Ŀ¼���Ƿ���ڵ�DACL������еĻ���
			// ��ô�ͽ�ָ���ʻ���ACE�ķ���Ȩ��ת�����������飬���ǿ��Կ���������ķ���
			// �ǲ���ACL_SIZE_INFORMATION�ṹ�е�AceCount��Ա����ɵġ������ѭ���У�
			// ���Һ�ָ���˻���ص�ACE
			// 
			if (fDaclPresent && AclInfo.AceCount) {
				for (CurrentAceIndex = 0; 
					CurrentAceIndex < AclInfo.AceCount;
					CurrentAceIndex++) {
						// 
						// STEP 10: ��DACL��ȡACE
						// 
						if (!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
							_tprintf(TEXT("GetAce() failed. Error %d\n"), 
								GetLastError());
							__leave;
						}
						// 
						// 
						// STEP 6: ���Ҫ������ACE��SID�Ƿ����Ҫ�����ACE��SIDһ����
						// ���һ������ô�ͽ���ACE�ķ���Ȩ��ת�����������飬
						// ����������������һ��ѭ��
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
							//bitset������������ֵ��˳���Ǵ�0��N-1
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
			// STEP 7: �ͷ��ѷ�����ڴ棬����Memory Leak
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
	// ����Ȩ��
	inline BOOL AddAccountRights(LPCWSTR lpszFileName, DWORD dwAccessMask) 
	{

		// ����SID����
		//SID_NAME_USE   snuType;
		// ������LookupAccountName��صı�����ע�⣬ȫΪ0��Ҫ�ڳ����ж�̬���䣩
		// 	LPVOID         pUserSID       = NULL;
		// 	DWORD          cbUserSID      = 0;
		// ���ļ���صİ�ȫ������ SD �ı���
		PSECURITY_DESCRIPTOR pFileSD  = NULL;     // �ṹ����
		DWORD          cbFileSD       = 0;        // SD��size
		// һ���µ�SD�ı��������ڹ����µ�ACL�������е�ACL����Ҫ�¼ӵ�ACL����������
		SECURITY_DESCRIPTOR  newSD;
		// ��ACL ��صı���
		PACL           pACL           = NULL;
		BOOL           fDaclPresent;
		BOOL           fDaclDefaulted;
		ACL_SIZE_INFORMATION AclInfo;
		// һ���µ� ACL ����
		PACL           pNewACL        = NULL;  //�ṹָ�����
		DWORD          cbNewACL       = 0;     //ACL��size
		// һ����ʱʹ�õ� ACE ����
		LPVOID         pTempAce       = NULL;
		UINT           CurrentAceIndex = 0;  //ACE��ACL�е�λ��
		UINT           newAceIndex = 0;  //�����ACE��ACL�е�λ��
		//API�����ķ���ֵ���������еĺ���������ʧ�ܡ�
		BOOL           fResult = FALSE;
		BOOL           fAPISuccess = FALSE;
		SECURITY_INFORMATION secInfo = DACL_SECURITY_INFORMATION;

		HANDLE hAccessToken;  
		BYTE * InfoBuffer = new BYTE[1024];  
		DWORD dwInfoBufferSize;  
		PSID pUsersid = NULL;  
		SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;  


		// ����������������µ�API����������Windows 2000���ϰ汾�Ĳ���ϵͳ֧�֡� 
		// �ڴ˽���Advapi32.dll�ļ��ж�̬���롣�����ʹ��VC++ 6.0������򣬶�������
		// ʹ�������������ľ�̬���ӡ�����Ϊ��ı�����ϣ�/D_WIN32_WINNT=0x0500
		// �ı������������ȷ�����SDK��ͷ�ļ���lib�ļ������µġ�
		SetSecurityDescriptorControlFnPtr _SetSecurityDescriptorControl = NULL;
		AddAccessAllowedAceExFnPtr _AddAccessAllowedAceEx = NULL; 
		__try {
			// 
			// STEP 1: ͨ���û���ȡ��SID
			//     ����һ����LookupAccountName���������������Σ���һ����ȡ������Ҫ
			// ���ڴ�Ĵ�С��Ȼ�󣬽����ڴ���䡣�ڶ��ε��ò���ȡ�����û����ʻ���Ϣ��
			// LookupAccountNameͬ������ȡ�����û������û������Ϣ������ο�MSDN��
			//

			// STEP 1: ȡ�õ�ǰ�û���SID
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
			// STEP 2: ȡ���ļ���Ŀ¼����صİ�ȫ������SD
			//     ʹ��GetFileSecurity����ȡ��һ���ļ�SD�Ŀ�����ͬ�����������Ҳ
			// �Ǳ��������Σ���һ��ͬ����ȡSD���ڴ泤�ȡ�ע�⣬SD�����ָ�ʽ������ص�
			// ��self-relative���� ��ȫ�ģ�absolute����GetFileSecurityֻ��ȡ������
			// ��صġ�����SetFileSecurity����Ҫ��ȫ�ġ������Ϊʲô��Ҫһ���µ�SD��
			// ������ֱ����GetFileSecurity���ص�SD�Ͻ����޸ġ���Ϊ������صġ���Ϣ
			// �ǲ������ġ�
			fAPISuccess = GetFileSecurity(lpszFileName,	secInfo, pFileSD, 0, &cbFileSD);
			// ���ϵ���API��ʧ�ܣ�ʧ��ԭ�����ڴ治�㡣��������Ҫ���ڴ��С������
			// �����Ǵ�����ڴ治��Ĵ���
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
			// STEP 3: ��ʼ��һ���µ�SD
			// 
			if (!InitializeSecurityDescriptor(&newSD, 
				SECURITY_DESCRIPTOR_REVISION)) {
					_tprintf(TEXT("InitializeSecurityDescriptor() failed.")
						TEXT("Error %d\n"), GetLastError());
					__leave;
			}
			// 
			// STEP 4: ��GetFileSecurity ���ص�SD��ȡDACL
			// 
			if (!GetSecurityDescriptorDacl(pFileSD, &fDaclPresent, &pACL,
				&fDaclDefaulted)) {
					_tprintf(TEXT("GetSecurityDescriptorDacl() failed. Error %d\n"),
						GetLastError());
					__leave;
			}
			// 
			// STEP 5: ȡ DACL���ڴ�size
			//     GetAclInformation�����ṩDACL���ڴ��С��ֻ����һ������Ϊ
			// ACL_SIZE_INFORMATION��structure�Ĳ�������DACL����Ϣ����Ϊ��
			// �������Ǳ������е�ACE��
			AclInfo.AceCount = 0; // Assume NULL DACL.
			AclInfo.AclBytesFree = 0;
			AclInfo.AclBytesInUse = sizeof(ACL);
			if (pACL == NULL)
				fDaclPresent = FALSE;
			// ���DACL��Ϊ�գ���ȡ����Ϣ�������������¡��Թ�������DACLΪ�գ�
			if (fDaclPresent) {            
				if (!GetAclInformation(pACL, &AclInfo, 
					sizeof(ACL_SIZE_INFORMATION), AclSizeInformation)) {
						_tprintf(TEXT("GetAclInformation() failed. Error %d\n"),
							GetLastError());
						__leave;
				}
			}
			// 
			// STEP 6: �����µ�ACL��size
			//    ����Ĺ�ʽ�ǣ�ԭ�е�DACL��size������Ҫ��ӵ�һ��ACE��size����
			// ������һ����ACE��ص�SID��size������ȥ�����ֽ��Ի�þ�ȷ�Ĵ�С��
			cbNewACL = AclInfo.AclBytesInUse + sizeof(ACCESS_ALLOWED_ACE) 
				+ GetLengthSid(pUsersid) - sizeof(DWORD);

			// 
			// STEP 7: Ϊ�µ�ACL�����ڴ�
			// 
			pNewACL = (PACL) (HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbNewACL));
			if (!pNewACL) {
				_tprintf(TEXT("HeapAlloc() failed. Error %d\n"), GetLastError());
				__leave;
			}
			// 
			// STEP 8: ��ʼ���µ�ACL�ṹ
			// 
			if (!InitializeAcl(pNewACL, cbNewACL, ACL_REVISION2)) {
				_tprintf(TEXT("InitializeAcl() failed. Error %d\n"), 
					GetLastError());
				__leave;
			} 

			// 
			// STEP 9  ����ļ���Ŀ¼�� DACL �����ݣ��������е�ACE���µ�DACL��
			// 
			//     ����Ĵ���������ȼ��ָ���ļ���Ŀ¼���Ƿ���ڵ�DACL������еĻ���
			// ��ô�Ϳ������е�ACE���µ�DACL�ṹ�У����ǿ��Կ���������ķ����ǲ���
			// ACL_SIZE_INFORMATION�ṹ�е�AceCount��Ա����ɵġ������ѭ���У�
			// �ᰴ��Ĭ�ϵ�ACE��˳�������п�����ACE��ACL�е�˳���Ǻܹؼ��ģ����ڿ�
			// �������У��ȿ����Ǽ̳е�ACE������֪��ACE����ϲ�Ŀ¼�м̳�������
			// 
			newAceIndex = 0;
			if (fDaclPresent && AclInfo.AceCount) {
				for (CurrentAceIndex = 0; 
					CurrentAceIndex < AclInfo.AceCount;
					CurrentAceIndex++) {
						// 
						// STEP 10: ��DACL��ȡACE
						// 
						if (!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
							_tprintf(TEXT("GetAce() failed. Error %d\n"), 
								GetLastError());
							__leave;
						}
						// 
						// STEP 11: ����Ƿ��ǷǼ̳е�ACE
						//     �����ǰ��ACE��һ���Ӹ�Ŀ¼�̳�����ACE����ô���˳�ѭ����
						// ��Ϊ���̳е�ACE�����ڷǼ̳е�ACE֮�󣬶�������Ҫ��ӵ�ACE
						// Ӧ�������еķǼ̳е�ACE֮�����еļ̳е�ACE֮ǰ���˳�ѭ��
						// ����Ϊ��Ҫ���һ���µ�ACE���µ�DACL�У���������ٰѼ̳е�
						// ACE�������µ�DACL�С�
						//
						if (((ACCESS_ALLOWED_ACE *)pTempAce)->Header.AceFlags
							& INHERITED_ACE)
							break;
						// 
						// STEP 12: ���Ҫ������ACE��SID�Ƿ����Ҫ�����ACE��SIDһ����
						// ���һ������ô��Ӧ�÷ϵ��Ѵ��ڵ�ACE��Ҳ����˵��ͬһ���û��Ĵ�ȡ
						// Ȩ�޵����õ�ACE����DACL��Ӧ��Ψһ�������������ͬһ�û�������
						// �˵�ACE�����ǿ��������û���ACE��
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
						// STEP 13: ��ACE���뵽�µ�DACL��
						//    ����Ĵ����У�ע�� AddAce �����ĵ����������������������˼�� 
						// ACL�е�����ֵ����ΪҪ��ACE�ӵ�ĳ����λ��֮�󣬲���MAXDWORD��
						// ��˼��ȷ����ǰ��ACE�Ǳ����뵽����λ�á�
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
			// STEP 14: ��һ�� access-allowed ��ACE ���뵽�µ�DACL��
			//     ǰ���ѭ�����������еķǼ̳���SIDΪ�����û���ACE���˳�ѭ���ĵ�һ����
			// ���Ǽ�������ָ����ACE����ע�������ȶ�̬װ����һ��AddAccessAllowedAceEx
			// ��API���������װ�ز��ɹ����͵���AddAccessAllowedAce������ǰһ��������
			// ��Windows 2000�Ժ�İ汾֧�֣�NT��û�У�����Ϊ��ʹ���°汾�ĺ�����������
			// ���ȼ��һ�µ�ǰϵͳ�пɲ�����װ���������������������ʹ�á�ʹ�ö�̬����
			// ��ʹ�þ�̬���ӵĺô��ǣ���������ʱ������Ϊû�����API����������
			// 
			// Ex��ĺ��������һ������AceFlag�������˲�������������������ǿ���������һ
			// ����ACE_HEADER�Ľṹ���Ա������������õ�ACE���Ա�����Ŀ¼���̳���ȥ���� 
			// AddAccessAllowedAce�������ܶ��������������AddAccessAllowedAce����
			// �У�����ACE_HEADER����ṹ���óɷǼ̳еġ�
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
			// STEP 15: �����Ѵ��ڵ�ACE��˳�򿽱��Ӹ�Ŀ¼�̳ж�����ACE
			// 
			std::bitset<32> bit((int)dwAccessMask);
			if (fDaclPresent && AclInfo.AceCount) {
				for (; 
					CurrentAceIndex < AclInfo.AceCount;
					CurrentAceIndex++) {
						// 
						// STEP 16: ���ļ���Ŀ¼����DACL�м���ȡACE
						// 
						if (!GetAce(pACL, CurrentAceIndex, &pTempAce)) {
							_tprintf(TEXT("GetAce() failed. Error %d\n"), 
								GetLastError());
							__leave;
						}
						// 
						// STEP 17: ��ACE���뵽�µ�DACL��
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
			// STEP 18: ���µ�ACL���õ��µ�SD��
			// 
			if (!SetSecurityDescriptorDacl(&newSD, TRUE, pNewACL, 
				FALSE)) {
					_tprintf(TEXT("SetSecurityDescriptorDacl() failed. Error %d\n"),
						GetLastError());
					__leave;
			}
			// 
			// STEP 19: ���ϵ�SD�еĿ��Ʊ���ٿ������µ�SD�У�����ʹ�õ���һ���� 
			// SetSecurityDescriptorControl() ��API�������������ͬ��ֻ������
			// Windows 2000�Ժ�İ汾�У��������ǻ���Ҫ��̬�ذ����advapi32.dll 
			// �����룬���ϵͳ��֧������������ǾͲ������ϵ�SD�Ŀ��Ʊ���ˡ�
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
			// STEP 20: ���µ�SD�������õ��ļ��İ�ȫ�����У�ǧɽ��ˮ�������ڵ��ˣ�
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
			// STEP 21: �ͷ��ѷ�����ڴ棬����Memory Leak
			// 
			if (pFileSD) (::HeapFree(GetProcessHeap(), 0, pFileSD));
			if (pNewACL) (::HeapFree(GetProcessHeap(), 0, pNewACL));
		}
		return fResult;

	}

	inline BOOL IsSamePath(LPCWSTR _szPath1, LPCWSTR _szPath2)
	{
		// �����־��Ч����Ӱ��,̫��ѭ��Ҫ����
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
