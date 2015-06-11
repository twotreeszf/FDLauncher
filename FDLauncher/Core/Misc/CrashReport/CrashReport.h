/* -------------------------------------------------------------------------
//	文件名		：	crashreport/crashreport.h
//	创建者		：	冰峰
//	创建时间	：	2009-8-5 17:01:13
//	功能描述	：	处理程序崩溃策略
//
//	$Id: $
// -----------------------------------------------------------------------*/
//#ifndef __CRASHREPORT_CRASHREPORT_H__
#ifdef __CRASHREPORT_CRASHREPORT_H__
#error "This File Only Can Include Once!"
#endif

#define __CRASHREPORT_CRASHREPORT_H__

#include <io.h>
#include <ImageHlp.h>
#include <assert.h>

typedef BOOL (WINAPI *PFN_MiniDumpWriteDump)(
						  IN HANDLE hProcess,
						  IN DWORD ProcessId,
						  IN HANDLE hFile,
						  IN MINIDUMP_TYPE DumpType,
						  IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, OPTIONAL
						  IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, OPTIONAL
						  IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);
typedef BOOL (*PFN_CallBack)(DWORD dwExceptionCode);

//////////////////////////////////////////////////////////////////////////
//

class CCrashReport
{
private:
	WCHAR				m_szOutputPath[MAX_PATH];
	HMODULE				m_hDebugHelp;

public:
	CCrashReport()
	{
		m_hDebugHelp		= NULL;
	}
	~CCrashReport()
	{
		if (m_hDebugHelp)
			::FreeLibrary(m_hDebugHelp);
	}
	static CCrashReport& Instance()
	{
		static CCrashReport _obj;		// 因为只可能被include 1次，所有是安全的
		return _obj;
	}

	HRESULT Init(LPCWSTR szOutputPath)
	{
		WCHAR szBuf[MAX_PATH] = { 0 };
		::GetModuleFileName(::GetModuleHandle(NULL), szBuf, MAX_PATH);
		if (szOutputPath)
		{
			wcscpy(m_szOutputPath, szOutputPath);
			wcscat(m_szOutputPath, L"dump\\");
			::CreateDirectoryW(m_szOutputPath, NULL);
			wcscat(m_szOutputPath, wcsrchr(szBuf, '\\') + 1);
		}
		else
		{
			wcscpy(m_szOutputPath, szBuf);
		}

		_TryToDeleteOldDumpFile();
		::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		return S_OK;
	}

	static LONG CALLBACK MyUnhandledExceptionFilter(EXCEPTION_POINTERS *lpTopLevelExceptionFilter)									   
	{
		// 加载debughlp.dll
		PFN_MiniDumpWriteDump pfnMiniDumpWriteDump;
		CCrashReport::Instance().m_hDebugHelp = ::LoadLibraryW(L"dbghelp.dll");
		if (!CCrashReport::Instance().m_hDebugHelp)
			goto Exit0;
		pfnMiniDumpWriteDump = (PFN_MiniDumpWriteDump)::GetProcAddress(CCrashReport::Instance().m_hDebugHelp, "MiniDumpWriteDump");
		if (!pfnMiniDumpWriteDump)
			goto Exit0;

		//生成 mini crash dump
		{
			HANDLE hDumpFile;
			WCHAR szFileName[MAX_PATH];

			{
				time_t now;
				time(&now);
				tm *ptm = localtime(&now);
				swprintf(szFileName, L"%s_%02d-%02d-%02d-%02d-%02d-%02d.dmp", 
					CCrashReport::Instance().m_szOutputPath,
					ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			}

			hDumpFile = ::CreateFileW(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
			if (hDumpFile != INVALID_HANDLE_VALUE)
			{
				MINIDUMP_EXCEPTION_INFORMATION ExpParam = { 0 };
				ExpParam.ThreadId = ::GetCurrentThreadId();
				ExpParam.ExceptionPointers = lpTopLevelExceptionFilter;
				ExpParam.ClientPointers = FALSE;
				pfnMiniDumpWriteDump(
					GetCurrentProcess(), GetCurrentProcessId(),
					hDumpFile, /*MiniDumpWithDataSegs*/ /*MiniDumpWithFullMemory*/ MiniDumpNormal, &ExpParam, NULL, NULL);
				Z_CLOSEHANDLE(hDumpFile);
			}
		}
Exit0:
		::ExitProcess(1);	// 不让系统弹崩溃对话框
		return EXCEPTION_EXECUTE_HANDLER;
	}

private:
	void _SimpleFormatExceptionInfo(EXCEPTION_RECORD* pExceptionRecord, LPWSTR szResult)
	{
		// ModuleName 属性。
		WCHAR szModuleName[MAX_PATH] = {0};
		::GetModuleFileName(::GetModuleHandle(NULL), szModuleName, MAX_PATH);

		// ExceptionDescription 属性
		LPCWSTR szExceptionDescription;
		switch (pExceptionRecord->ExceptionCode)
		{
		case EXCEPTION_ACCESS_VIOLATION:
			szExceptionDescription = L"EXCEPTION_ACCESS_VIOLATION";
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			szExceptionDescription = L"EXCEPTION_DATATYPE_MISALIGNMENT";
			break;
		case EXCEPTION_BREAKPOINT:
			szExceptionDescription = L"EXCEPTION_BREAKPOINT";
			break;
		case EXCEPTION_SINGLE_STEP:
			szExceptionDescription = L"EXCEPTION_SINGLE_STEP";
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			szExceptionDescription = L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			szExceptionDescription = L"EXCEPTION_FLT_DENORMAL_OPERAND";
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			szExceptionDescription = L"EXCEPTION_FLT_DIVIDE_BY_ZERO";
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			szExceptionDescription = L"EXCEPTION_FLT_INEXACT_RESULT";
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			szExceptionDescription = L"EXCEPTION_FLT_INVALID_OPERATION";
			break;
		case EXCEPTION_FLT_OVERFLOW :
			szExceptionDescription = L"EXCEPTION_FLT_OVERFLOW ";
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			szExceptionDescription = L"EXCEPTION_FLT_STACK_CHECK";
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			szExceptionDescription = L"EXCEPTION_FLT_UNDERFLOW";
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			szExceptionDescription = L"EXCEPTION_INT_DIVIDE_BY_ZERO";
			break;
		case EXCEPTION_INT_OVERFLOW:
			szExceptionDescription = L"EXCEPTION_INT_OVERFLOW";
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			szExceptionDescription = L"EXCEPTION_PRIV_INSTRUCTION";
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			szExceptionDescription = L"EXCEPTION_IN_PAGE_ERROR";
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			szExceptionDescription = L"EXCEPTION_ILLEGAL_INSTRUCTION";
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			szExceptionDescription = L"EXCEPTION_NONCONTINUABLE_EXCEPTION";
			break;
		case EXCEPTION_STACK_OVERFLOW:
			szExceptionDescription = L"EXCEPTION_STACK_OVERFLOW";
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			szExceptionDescription = L"EXCEPTION_INVALID_DISPOSITION";
			break;
		case EXCEPTION_GUARD_PAGE:
			szExceptionDescription = L"EXCEPTION_GUARD_PAGE";
			break;
		case EXCEPTION_INVALID_HANDLE:
			szExceptionDescription = L"EXCEPTION_INVALID_HANDLE";
			break;
		default:
			szExceptionDescription = L"EXCEPTION_UNKNOWN";   // 未知的异常。
			break;
		}
		
		swprintf(szResult,
			L"Module=%s\nCode=%x\nDescription=%s\nAdddress=%x",
			szModuleName,
			pExceptionRecord->ExceptionCode,
			szExceptionDescription,
			pExceptionRecord->ExceptionAddress);
	}

	void _TryToDeleteOldDumpFile()
	{		
		time_t now = time(&now);
		tm *ptm = localtime(&now);
		
		// Dump路径
		WCHAR szDumpPath[MAX_PATH] = { 0 };
		wcscpy(szDumpPath, m_szOutputPath);
		*wcsrchr(szDumpPath, '\\') = 0;
		wcscat(szDumpPath, L"\\*.*");

		_wfinddata_t fd;
		long hFind = _wfindfirst(szDumpPath, &fd);		// 至少有"."和".."
		while (hFind != -1)
		{
			if (!(fd.attrib & _A_SUBDIR))
			{
				if (fd.time_create < now - 3 * 24 * 60 * 60)
				{
					WCHAR szTemp[MAX_PATH];
					wcscpy(szTemp, m_szOutputPath);
					*wcsrchr(szTemp, '\\') = 0;
					wcscat(szTemp, L"\\");
					wcscat(szTemp, fd.name);
					BOOL bRet = ::DeleteFileW(szTemp);
					assert(bRet);
				}
			}

			if (_wfindnext(hFind, &fd) == -1)
				break;
		}
		
		if (hFind != -1)
			_findclose(hFind);
	}
};


// -------------------------------------------------------------------------
//	$Log: $

//#endif /* __CRASHREPORT_CRASHREPORT_H__ */
