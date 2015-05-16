/* -------------------------------------------------------------------------
// 文件名		:	framework/errorcheck.h
// 创建人		:	冰峰
// 创建时间		:	2008-5-31 星期六 10:50:49
// 功能描述     :	Error Check
//
// $Id: $
// -----------------------------------------------------------------------*/
#ifndef __FRAMEWORK_ERRORCHECK_H__
#define __FRAMEWORK_ERRORCHECK_H__



//////////////////////////////////////////////////////////////////////////
//
#undef ASSERT
#undef CHECK_BOOL
#undef CHECK_BOOLEX

//////////////////////////////////////////////////////////////////////////
// 日志策略
#ifdef USING_LOG
#define ERROR_LOG(x)				LOG_ERROR(x)
#define INFO_LOG(x)					LOG_INFO(x)
#define WARNING_LOG(x)				LOG_WARNING(x)
#define REPEAT_LOG(x)				LOG_REPEAT(x)
#define TRACE_LOG(x)				LOG_TRACE(x)

#define ERROR_LOG2(x1, x2)			LOG_ERROR2(x1, x2)
#define INFO_LOG2(x1, x2)			LOG_INFO2(x1, x2)
#define WARNING_LOG2(x1, x2)		LOG_WARNING2(x1, x2)
#define REPEAT_LOG2(x1, x2)			LOG_REPEAT2(x1, x2)
#define TRACE_LOG2(x1, x2)			LOG_TRACE2(x1, x2)

#define ERROR_LOG3(x1, x2, x3)		LOG_ERROR3(x1, x2, x3)
#define INFO_LOG3(x1, x2, x3)		LOG_INFO3(x1, x2, x3)
#define WARNING_LOG3(x1, x2, x3)	LOG_WARNING3(x1, x2, x3)
#define REPEAT_LOG3(x1, x2, x3)		LOG_REPEAT3(x1, x2, x3)
#define TRACE_LOG3(x1, x2, x3)		LOG_TRACE3(x1, x2, x3)

#define ERROR_LOG4(x1, x2, x3, x4)		LOG_ERROR4(x1, x2, x3, x4)
#define INFO_LOG4(x1, x2, x3, x4)		LOG_INFO4(x1, x2, x3, x4)
#define WARNING_LOG4(x1, x2, x3, x4)	LOG_WARNING4(x1, x2, x3, x4)
#define REPEAT_LOG4(x1, x2, x3, x4)		LOG_REPEAT4(x1, x2, x3, x4)
#define TRACE_LOG4(x1, x2, x3, x4)		LOG_TRACE4(x1, x2, x3, x4)

#define ERROR_LOG5(x1, x2, x3, x4, x5)		LOG_ERROR5(x1, x2, x3, x4, x5)

#define TRACE_ONCE_LOG(x1)												\
do {																	\
	static int trigger = 0;												\
	if (!trigger) {														\
		TRACE_LOG(x1);													\
		trigger = 1;													\
	}																	\
} while(0)

#define TRACE_ONCE_LOG2(x1, x2)											\
do {																	\
	static int trigger = 0;												\
	if (!trigger) {														\
		TRACE_LOG2(x1, x2);												\
		trigger = 1;													\
	}																	\
} while(0)

#define TRACE_ONCE_LOG3(x1, x2, x3)										\
	do {																\
	static int trigger = 0;												\
	if (!trigger) {														\
		TRACE_LOG3(x1, x2, x3);											\
		trigger = 1;													\
	}																	\
} while(0)

#else
#define ERROR_LOG(x)				((void)0)
#define INFO_LOG(x)					((void)0)
#define WARNING_LOG(x)				((void)0)
#define REPEAT_LOG(x)				((void)0)
#define TRACE_LOG(x)				((void)0)

#define ERROR_LOG2(x1, x2)			((void)0)
#define INFO_LOG2(x1, x2)			((void)0)
#define WARNING_LOG2(x1, x2)		((void)0)
#define REPEAT_LOG2(x1, x2)			((void)0)
#define TRACE_LOG2(x1, x2)			((void)0)
#define ERROR_LOG3(x1, x2, x3)				((void)0)
#define ERROR_LOG4(x1, x2, x3, x4)			((void)0)
#define ERROR_LOG5(x1, x2, x3, x4, x5)		((void)0)

#define INFO_LOG3(x1, x2, x3)		((void)0)
#define WARNING_LOG3(x1, x2, x3)	((void)0)
#define REPEAT_LOG3(x1, x2, x3)		((void)0)
#define TRACE_LOG3(x1, x2, x3)		((void)0)

#define TRACE_ONCE_LOG(x1)			((void)0)
#define TRACE_ONCE_LOG2(x1, x2)		((void)0)

#endif

//////////////////////////////////////////////////////////////////////////
// ASSERT策略
#define ASSERT_ONCE(f)													\
do {																	\
	static int trigger = 0;												\
	if (!trigger && !(f)) {												\
		ATLASSERT(!#f);													\
		trigger = 1;													\
	}																	\
} while(0)

#if defined FORCE_ASSERT_ONCE && defined _DEBUG
	#define Z_ASSERT(f)				ASSERT_ONCE(f)
#elif defined _DEBUG
	#define Z_ASSERT(f)				ATLASSERT(f)
#else
	#define Z_ASSERT(f)													\
	do																	\
	{																	\
		if (!(f))														\
			ERROR_LOG(L"*ASSERT*: " L#f);								\
	} while (0);
#endif

#ifdef DISABLE_ASSERT
	#define ASSERT(f)				((void)0)
	#define ASSERT_W(f)				((void)0)
    #define XLIVE_ASSERT(f)         ((void)0)
#else
	#define ASSERT(f)				Z_ASSERT(f)
#include <crtdbg.h>
    #define XLIVE_ASSERT(f)         _ASSERT(f)


// 警告ASSERT，上面的Z_ASSERT是契约ASSERT
#define ASSERT_W(f)					ATLASSERT(f)
#endif

#define ASSERT_COM(hr)				ASSERT(SUCCEEDED(hr))

//////////////////////////////////////////////////////////////////////////
// Path Control
#define CHECK_BOOL(exp)														\
do {																		\
	if (!(exp))																\
	{																		\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_BOOL2(exp, msg)												\
do {																		\
	if (!(exp))																\
	{																		\
		TRACE_LOG2(L"CHECK_BOOL:" L#exp L" MSG: %s", msg);					\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_BOOL3(exp, fmt, msg1)											\
do {																		\
	if (!(exp))																\
	{																		\
		TRACE_LOG2(L"CHECK_BOOL:" L#exp L" MSG: " L#fmt, msg1);				\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOL(exp)												\
do {																		\
	if (!(exp))																\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK Faild!"));								\
		WARNING_LOG(L"ERROR_CHECK_BOOL:" L#exp);							\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_BOOLEX(exp, exp1)												\
do {																		\
	if (!(exp))																\
	{																		\
		exp1;																\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_BOOLEX2(exp, exp1, msg)										\
do {																		\
	if (!(exp))																\
	{																		\
		exp1;																\
		TRACE_LOG2(L"CHECK_BOOLEX:" L#exp L" MSG: %s", msg);				\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_BOOLEX3(exp, exp1, fmt, msg1)									\
do {																		\
	if (!(exp))																\
	{																		\
		exp1;																\
		TRACE_LOG2(L"CHECK_BOOLEX:" L#exp L" MSG:" L#fmt, msg1);			\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOLEX(exp, exp1)										\
do {																		\
	if (!(exp))																\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK Faild!"));								\
		exp1;																\
		WARNING_LOG(L"ERROR_CHECK_BOOLEX:" L#exp);							\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOLEX_NOLOG(exp, exp1)										\
	do {																	\
		if (!(exp))															\
		{																	\
			ASSERT_W(!_T("ERROR_CHECK Faild!"));							\
			exp1;															\
			goto Exit0;														\
		}																	\
	} while(0)

#define CHECK_COM(exp)														\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		TRACE_LOG2(L"CHECK_COM hr = %x: " L#exp, hr);						\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_COM_NOLOG(exp)												\
	do {																	\
	if (!SUCCEEDED(exp))													\
		{																	\
			goto Exit0;														\
		}																	\
	} while(0)

#define CHECK_COM2(exp, msg)												\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		TRACE_LOG3(L"CHECK_COM2:" L#exp L" hr = %x, MSG: %s", hr, msg);		\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_COM(exp)												\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK_COM Faild!"));							\
		WARNING_LOG2(L"ERROR_CHECK_COM hr = %x: " L#exp, hr);				\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_COMEX(exp, exp1)										\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK_COMEX Faild!"));							\
		exp1;																\
		WARNING_LOG2(L"ERROR_CHECK_COMEX hr = %x: " L#exp, hr);				\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_COMEX_NOLOG(exp)										\
	do {																	\
		if (!SUCCEEDED(exp))												\
		{																	\
			ASSERT_W(!_T("ERROR_CHECK_COMEX Faild!"));						\
			goto Exit0;														\
		}																	\
	} while(0)
//////////////////////////////////////////////////////////////////////////
// Control Msg(Output Warning Log)
#define CHECK_BOOL_MSG(exp, msg)											\
do {																		\
	if (!(exp))																\
	{																		\
		WARNING_LOG2(L"CHECK_BOOL:" L#exp L"MSG: %s", msg);					\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOL_MSG(exp, msg)										\
do {																		\
	if (!(exp))																\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK Faild!"));								\
		WARNING_LOG2(L"ERROR_CHECK_BOOL:" L#exp L"MSG: %s", msg);			\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_BOOLEX_MSG(exp, exp1, msg)									\
do {																		\
	if (!(exp))																\
	{																		\
		exp1;																\
		WARNING_LOG2(L"CHECK_BOOLEX:" L#exp L"MSG: %s", msg);				\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOLEX_MSG(exp, exp1, msg)								\
do {																		\
	if (!(exp))																\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK Faild!"));								\
		exp1;																\
		WARNING_LOG2(L"ERROR_CHECK_BOOLEX:" L#exp L"MSG: %s", msg);			\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_COM_MSG(exp, msg)												\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		TRACE_LOG(L"CHECK_COM:" L#exp L"hr = %x, MSG: %s", hr, msg);		\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_COM_MSG(exp, msg)										\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK_COM Faild!"));							\
		WARNING_LOG3(L"ERROR_CHECK_COM:" L#exp L"hr = %x, MSG: %s", hr, msg);\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_COM_MSG2(exp, fmt, msg1)								\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK_COM Faild!"));							\
		WARNING_LOG3(L"ERROR_CHECK_COM: hr = %x, " L#exp L#fmt, hr, msg1);	\
		goto Exit0;																\
	}																		\
} while(0)

//////////////////////////////////////////////////////////////////////////
// Control Msg(Output Info Log)
#define CHECK_BOOL_INFO(exp, msg)											\
do {																		\
	if (!(exp))																\
	{																		\
		INFO_LOG2(L"CHECK_BOOL:" L#exp L"MSG: %s", msg);						\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOL_INFO(exp, msg)										\
do {																		\
	if (!(exp))																\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK Faild!"));								\
		INFO_LOG2(L"ERROR_CHECK_BOOL:" L#exp L"MSG: %s", msg);				\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_BOOLEX_INFO(exp, exp1, msg)									\
do {																		\
	if (!(exp))																\
	{																		\
		exp1;																\
		INFO_LOG2(L"CHECK_BOOLEX:" L#exp L"--MSG:%s", msg);					\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOLEX_INFO(exp, exp1, msg)								\
do {																		\
	if (!(exp))																\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK Faild!"));								\
		exp1;																\
		INFO_LOG2(L"ERROR_CHECK_BOOLEX:" L#exp L"--MSG:%s", msg);			\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_COM_INFO(exp, msg)											\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		INFO_LOG3(L"CHECK_COM:" L#exp L" hr = %x, MSG:%s", hr, msg);		\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_COM_INFO(exp, msg)										\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK_COM Faild!"));							\
		INFO_LOG3(L"ERROR_CHECK_COM:" L#exp L" hr = %x, MSG:%s", hr, msg);	\
		goto Exit0;															\
	}																		\
} while(0)

//////////////////////////////////////////////////////////////////////////
// Control Msg(Output ERROR Log)
#define CHECK_BOOL_ERR(exp, msg)											\
do {																		\
	if (!(exp))																\
	{																		\
		ERROR_LOG2(L"CHECK_BOOL:" L#exp L"--MSG:%s", msg);					\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_BOOL_ERR2(exp, fmt, msg1)										\
do {																		\
	if (!(exp))																\
	{																		\
		ERROR_LOG2(L"CHECK_BOOL:" L#exp L"--MSG:" L#fmt, msg1);				\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOL_ERR(exp, msg)										\
do {																		\
	if (!(exp))																\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK Faild!"));								\
		ERROR_LOG2(L"ERROR_CHECK_BOOL:" L#exp L"--MSG:%s", msg);			\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_BOOLEX_ERR(exp, exp1, msg)									\
do {																		\
	if (!(exp))																\
	{																		\
		exp1;																\
		ERROR_LOG2(L"CHECK_BOOLEX:" L#exp L"--MSG:%s", msg);				\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_BOOLEX_ERR2(exp, exp1, fmt, msg1)								\
do {																		\
	if (!(exp))																\
	{																		\
		exp1;																\
		ERROR_LOG2(L"CHECK_BOOLEX:" L#exp L"--MSG:" L#fmt, msg1);			\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOLEX_ERR(exp, exp1, msg)								\
do {																		\
	if (!(exp))																\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK Faild!"));								\
		exp1;																\
		ERROR_LOG2(L"ERROR_CHECK_BOOLEX:" L#exp L"MSG: %s", msg);			\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOLEX_ERR2(exp, exp1, fmt, msg1)						\
do {																		\
	if (!(exp))																\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK Faild!"));								\
		exp1;																\
		ERROR_LOG2(L"ERROR_CHECK_BOOLEX:" L#exp L#fmt, msg1);				\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_BOOLEX_ERR3(exp, exp1, fmt, msg1, msg2)					\
do {																		\
	if (!(exp))																\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK Faild!"));								\
		exp1;																\
		ERROR_LOG3(L"ERROR_CHECK_BOOLEX:" L#exp L#fmt, msg1, msg2);			\
		goto Exit0;															\
	}																		\
} while(0)

#define CHECK_COM_ERR(exp, msg)												\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		ERROR_LOG3(L"CHECK_COM: hr = %x MSG: %s", hr, msg);					\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_COM_ERR(exp, msg)										\
do {																		\
	if (!SUCCEEDED(exp))													\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK_COM Faild!"));							\
		ERROR_LOG3(L"ERROR_CHECK_COM: hr = %x MSG: %s", hr, msg);			\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_COM_ERR2(hr, fmt, msg1)									\
do {																		\
	if (!SUCCEEDED(hr))														\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK_COM Faild!"));							\
		ERROR_LOG3(L"ERROR_CHECK_COM: hr = %x :" L#fmt, hr, msg1);			\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_COM_ERR3(hr, fmt, msg1, msg2)							\
	do {																	\
	if (!SUCCEEDED(hr))														\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK_COM Faild!"));							\
		ERROR_LOG4(L"ERROR_CHECK_COM: hr = %x :" L#fmt, hr, msg1, msg2);	\
		goto Exit0;															\
	}																		\
} while(0)

#define ERROR_CHECK_COM_ERR4(hr, fmt, msg1, msg2, msg3)						\
do {																		\
	if (!SUCCEEDED(hr))														\
	{																		\
		ASSERT_W(!_T("ERROR_CHECK_COM Faild!"));							\
		ERROR_LOG5(L"ERROR_CHECK_COM: hr = %x :" L#fmt, hr, msg1, msg2, msg3);\
		goto Exit0;															\
	}																		\
} while(0)

//////////////////////////////////////////////////////////////////////////
// Log Helper
#define LOG_BOOL_ERROR(exp, msg)											\
do {																		\
	if (!(exp))																\
	{																		\
		ERROR_LOG2(L#exp L"--MSG:%s", msg);									\
	}																		\
} while(0)

#define LOG_BOOL_INFO(exp, msg)												\
do {																		\
	if (!(exp))																\
	{																		\
		INFO_LOG2(L#exp L"--MSG:%s", msg);									\
	}																		\
} while(0)

#define LOG_BOOL_WARNING(exp, msg)											\
do {																		\
	if (!(exp))																\
	{																		\
		WARNING_LOG2(L#exp L"--MSG:%s", msg);								\
	}																		\
} while(0)

#define LOG_BOOL_REPEAT(exp, msg)											\
do {																		\
	if (!(exp))																\
	{																		\
		REPEAT_LOG2(L#exp L"--MSG:%s", msg);								\
	}																		\
} while(0)

#define LOG_BOOL_TRACE(exp, msg)											\
do {																		\
	if (!(exp))																\
	{																		\
		TRACE_LOG2(L#exp L"--MSG:%s", msg);									\
	}																		\
} while(0)

#define LOG_COM_ERROR(exp, msg)			LOG_BOOL_ERROR(SUCCEEDED(exp), msg)
#define LOG_COM_INFO(exp, msg)			LOG_BOOL_INFO(SUCCEEDED(exp), msg)
#define LOG_COM_WARNING(exp, msg)		LOG_BOOL_WARNING(SUCCEEDED(exp), msg)
#define LOG_COM_REPEAT(exp, msg)		LOG_BOOL_REPEAT(SUCCEEDED(exp), msg)
#define LOG_COM_TRACE(exp, msg)			LOG_BOOL_TRACE(SUCCEEDED(exp), msg)

//////////////////////////////////////////////////////////////////////////
//

class _CTraceStack
{
private:
	DWORD			m_dwTickCount;
	CString			m_strName;
	HRESULT*		m_pHr;
	BOOL			m_bOutputToLog;

	LPCSTR			m_szFile;
	int				m_nLine;
	
public:
	static CString FormatArgs(LPCWSTR szFormat, ...)
	{
		va_list args;
		va_start(args, szFormat);
		WCHAR szMsg[1024] = { 0 };									// if被截断了，最后是没有0结束的 //  [10/26/2009 冰峰]
		int nBuf = _vsnwprintf(szMsg, 1023, szFormat, args);		// ASSERT(nBuf < 1023)
		va_end(args);
		return szMsg;
	}
	_CTraceStack(LPCSTR szFILE, int szLINE, LPCWSTR szName, HRESULT *pHr = NULL, LPCWSTR szArgs = NULL, BOOL bOutputToLog = TRUE)
	{
		m_szFile = szFILE;
		m_nLine	  = szLINE;
		m_strName = szName;
		m_pHr = pHr;
		m_bOutputToLog = bOutputToLog;
		m_dwTickCount = ::GetTickCount();

		if (m_bOutputToLog)
		{
			#ifdef USING_LOG
			Log_WriteLog(LOGLEVEL_TRACE, m_szFile, m_nLine, L"CTraceStack In, %s : %s", (LPCWSTR)m_strName, szArgs);
			#endif
		}
		else
		{
			CString str;
			str.Format(L"CTraceStack In, %s : %s\n", (LPCWSTR)m_strName, szArgs);
			::OutputDebugStringW(str);
		}
	}
	~_CTraceStack()
	{
		DWORD dwSpace = ::GetTickCount() - m_dwTickCount;
		CString strTemp;
		if (m_pHr)
			strTemp.Format(L"CTraceStack Out,%s : hr = %x, UserTime: %d'.%d'", (LPCWSTR)m_strName, *m_pHr, dwSpace / 1000, dwSpace % 1000);
		else
			strTemp.Format(L"CTraceStack Out,%s : UserTime: %d'.%d''", (LPCWSTR)m_strName, dwSpace / 1000, dwSpace % 1000);

		if (m_bOutputToLog)
		{
			#ifdef USING_LOG
			Log_WriteLog(LOGLEVEL_TRACE, m_szFile, m_nLine, strTemp);
			#endif
		}
		else
		{
			::OutputDebugStringW(strTemp);
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// 简单记录性能的工具
#if defined _DEBUG || defined RECORD_PERFORMANCE
class _CTickCount
{
private:
	DWORD			m_dwTickCount;
	CString			m_strName;

public:
	_CTickCount(LPCWSTR szName)
	{
		m_strName		= szName;
		m_dwTickCount	= ::GetTickCount();
	}
	~_CTickCount()
	{
		DWORD dwSpace = ::GetTickCount() - m_dwTickCount;
		ATLTRACE(L"TickCount,%s :%d'.%d''\n", (LPCWSTR)m_strName, dwSpace / 1000, dwSpace % 1000);

#ifdef USING_LOG
		LOG_TRACE4(L"TickCount,%s :%d'.%d''", (LPCWSTR)m_strName, dwSpace / 1000, dwSpace % 1000);
#endif
	}
};

// 使用高性能计数器来计算时间,
class _CAccurateTimeRecord
{
private:
	LARGE_INTEGER m_liStart;
	LARGE_INTEGER m_liEnd;
	LARGE_INTEGER m_Freq;
	CString       m_strMsg;
	BOOL		  m_bSupport;

public:
	_CAccurateTimeRecord(LPCWSTR pszMsg)
	{
		m_strMsg = pszMsg;
		
		// 检查系统硬件是否支持
		if (::QueryPerformanceFrequency(&m_Freq) && m_Freq.QuadPart)
		{
			m_bSupport = TRUE;
		}
		else
		{
			m_bSupport = FALSE;
			ATLTRACE(L"System Hardware Not Support");
#ifdef USING_LOG
			LOG_TRACE(L"System Hardware Not Support");
#endif
		}

		if (m_bSupport)
		{
			::QueryPerformanceCounter(&m_liStart);
		}
	}

	~_CAccurateTimeRecord()
	{
		if (m_bSupport)
		{
			::QueryPerformanceCounter(&m_liEnd);
			_CalTime();
		}

	}
private:
	inline void _CalTime()
	{
		double dTime = double(m_liEnd.QuadPart - m_liStart.QuadPart);
		dTime = dTime / m_Freq.QuadPart;
		
		ATLTRACE(L"MSG: %s ;CostTime: %.3fms", m_strMsg, dTime * 1000);
#ifdef USING_LOG
		LOG_TRACE3(L"MSG: %s ;CostTime: %.3fms", m_strMsg, dTime * 1000);
#endif
	}
};

#endif

#if defined _DEBUG || defined RECORD_PERFORMANCE
	#define RECORD_TRACE_STACK(x1, x2, x3)			_CTraceStack __z_record_trace_stack__(__FILE__, __LINE__, x1, x2, x3)
	#define RECORD_TRACE_STACK_DEBUG(x1, x2, x3)	_CTraceStack __z_record_trace_stack__(__FILE__, __LINE__, x1, x2, x3, FALSE)
	#define RECORD_TICK_COUNT(x)					_CTickCount __z_record_tick_count__(x)
	#define RECORD_HIGH_RESOLUTION_TIME(x)			_CAccurateTimeRecord __z_record_accurate_time(x)
	#define RECORD_TRACE_STACK2(x1, x2)				_CTraceStack __z_record_trace_stack__(__FILE__, __LINE__, __FUNCTIONW__, x1, x2)
	#define RECORD_TRACE_STACK_DEBUG2(x1, x2)		_CTraceStack __z_record_trace_stack__(__FILE__, __LINE__, __FUNCTIONW__, x1, x2, FALSE)
	#define RECORD_TICK_COUNT2()					_CTickCount __z_record_tick_count__(__FUNCTIONW__)
#else
	#define RECORD_TRACE_STACK(x1, x2, x3)			(void)0
	#define RECORD_TRACE_STACK_DEBUG(x1, x2, x3)	(void)0
	#define RECORD_TICK_COUNT(x)					(void)0
	#define RECORD_HIGH_RESOLUTION_TIME(x)          (void)0
	#define RECORD_TRACE_STACK2(x1, x2)				(void)0
	#define RECORD_TRACE_STACK_DEBUG2(x1, x2)		(void)0
#endif

// -------------------------------------------------------------------------
// $Log: $

#endif /* __FRAMEWORK_ERRORCHECK_H__ */
