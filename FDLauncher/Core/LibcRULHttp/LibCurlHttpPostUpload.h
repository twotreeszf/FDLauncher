/* -------------------------------------------------------------------------
//	File Name	:	LibCurlHttpPostUpload.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-21 9:51:41
//	Description	:
//
// -----------------------------------------------------------------------*/

#ifndef __LIBCURLHTTPPOSTUPLOAD_H__
#define __LIBCURLHTTPPOSTUPLOAD_H__

#include "CrossPlatformConfig.h"
#include "../Misc/ErrorCode.h"
#include "LibCurlHttpBase.h"
#include "FileProgressDelegate.h"
#include <filesystem>

// -------------------------------------------------------------------------

namespace LibCurl
{
    class CHttpPostUpload
        : public CHttpBase
    {
    private:
        UInt32                  m_lowSpeedTimeoutSec;
        FILE*                   m_pFile;
        UInt32                  m_fileBufferSize;

        CancelDelegate          m_cancel;
        FileProgressDelegate    m_progress;

    public:
        explicit CHttpPostUpload(const TransferSetting &transferSetting);
        CHttpPostUpload();
        ~CHttpPostUpload();

    public:
        void setProgressDelegate(FileProgressDelegate progress);
        virtual void setCancelDelegate(LibCurl::CancelDelegate cancel);
        void setFileBufferSize(UInt32 bufferSize);

        ErrorCode uploadFile(const char* url, const std::tr2::sys::path& file, UInt32 lowSpeedTimeoutSec = 5 * 60);

    private:
        virtual void _curlSetDIY();

    private:
        static size_t _readDataFunction(void* outBuffer, size_t size, size_t n, void* dataStream);
        bool _cancelDelegate();
    };
}

//--------------------------------------------------------------------------
#endif /* __LIBCURLHTTPPOSTUPLOAD_H__ */