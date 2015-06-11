/* -------------------------------------------------------------------------
//  File Name   :   TraceStack.h
//  Author      :   zhang fan
//  Create Time :   2015/06/11 11:36:28
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __TRACESTACK_H__
#define __TRACESTACK_H__

#include <chrono>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

//-------------------------------------------------------------------------

struct _TraceStack
{
	typedef std::chrono::time_point<std::chrono::system_clock> timep;
	std::string m_file;
	UInt32      m_line;
	std::string m_function;
	timep		m_startTime;
	UInt32*     m_pRet;

	_TraceStack(const std::string& function, const std::string& file, UInt32 line, UInt32* pRet = NULL);
	~_TraceStack();
};

#ifdef _MSC_VER
#define __X_FUNCTION__ __FUNCTION__
#else
#define __X_FUNCTION__ __PRETTY_FUNCTION__
#endif

#define TRACE_STACK \
_TraceStack __trace_stack__(__X_FUNCTION__, __FILENAME__, __LINE__)
//--------------------------------------------------------------------------
#endif /* __TRACESTACK_H__ */
