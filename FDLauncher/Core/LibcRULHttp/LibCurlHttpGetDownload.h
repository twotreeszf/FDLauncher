/* -------------------------------------------------------------------------
//  File Name   :   LibCurlHttpGetDownload.h
//  Author      :   Zhang Fan
//  Create Time :   2012-9-4 9:19:39
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __LIBCURLHTTPGETDOWNLOAD_H__
#define __LIBCURLHTTPGETDOWNLOAD_H__

//-------------------------------------------------------------------------
#include "../Misc/CrossPlatformConfig.h"
#include "LibCurlHttpBase.h"
#include "FileProgressDelegate.h"

//-------------------------------------------------------------------------

namespace LibCurl
{
	class CHttpGetDownload
		: public CHttpBase
	{
	private:
		UInt32                  m_lowSpeedTimeoutSec;
		FILE*                   m_pFile;

		CancelDelegate          m_cancel;
		FileProgressDelegate    m_progress;
        UInt32                  m_speedFactor;

	public:
		explicit CHttpGetDownload(const TransferSetting &transferSetting);
		CHttpGetDownload();
		~CHttpGetDownload();

	public:
		void setProgressDelegate(FileProgressDelegate progress);
        void setSpeedFactor(UInt32 factor);
		virtual void setCancelDelegate(LibCurl::CancelDelegate cancel);

		ErrorCode downloadFile(const char* url, const std::string& file, UInt32 lowSpeedTimeoutSec = 5 * 60);

	private:
		virtual void _curlSetRecvDataFunction();
		virtual void _curlSetDIY();
		bool _cancelDelegate();
	};
}



//--------------------------------------------------------------------------
#endif /* __LIBCURLHTTPGETDOWNLOAD_H__ */