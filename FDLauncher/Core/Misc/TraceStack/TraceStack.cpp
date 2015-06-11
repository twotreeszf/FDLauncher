/* -------------------------------------------------------------------------
//  File Name   :   TraceStack.cpp
//  Author      :   zhang fan
//  Create Time :   2015/06/11 11:36:31
//  Description :
//
// -----------------------------------------------------------------------*/

#include "stdafx.h"
#include "TraceStack.h"
#include "..\StringHelper.h"

//-------------------------------------------------------------------------

_TraceStack::_TraceStack(const std::string& function, const std::string& file, UInt32 line, UInt32* pRet /*= NULL*/)
	: m_function(function)
	, m_file(file)
	, m_line(line)
	, m_startTime(std::chrono::system_clock::now())
	, m_pRet(pRet)
{
	std::string head = StringHelper::format("%-18s", "[IN]");
	std::string log = StringHelper::format("%s%s", head.c_str(), m_function.c_str());

	LOG(INFO) << log << " " << m_file << "(" << m_line << ")" << std::endl;
}

_TraceStack::~_TraceStack()
{
	timep timeNow = std::chrono::system_clock::now();
	std::chrono::duration<double> duration = timeNow - m_startTime;

	std::string head = StringHelper::format("[OUT %f]", duration.count());
	head = StringHelper::format("%-18s", head.c_str());

	std::string log = StringHelper::format("%s%s", head.c_str(), m_function.c_str());
	LOG(INFO) << log << " " << m_file << "(" << m_line << ")" << std::endl;
}

//--------------------------------------------------------------------------         
