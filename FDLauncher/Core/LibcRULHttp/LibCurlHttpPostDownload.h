/* -------------------------------------------------------------------------
//  File Name   :   LibCurlHttpPostDownload.h
//  Author      :   Zhang Fan
//  Create Time :   2012-3-21 15:18:23
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __LIBCURLHTTPPOSTDOWNLOAD_H__
#define __LIBCURLHTTPPOSTDOWNLOAD_H__

#include "../Misc/CrossPlatformConfig.h"
#include "LibCurlHttpBase.h"
#include "FileProgressDelegate.h"

//-------------------------------------------------------------------------

namespace LibCurl
{
    class CHttpPostDownload
        : public CHttpBase
    {
    private:
        bool                    m_isBinaryMode;
        std::string             m_postData;
        UInt32                  m_lowSpeedTimeoutSec;
        FILE*                   m_pFile;
        UInt32                  m_fileBufferSize;

        CancelDelegate          m_cancel;
        FileProgressDelegate    m_progress;

    public:
        explicit CHttpPostDownload(const TransferSetting &transferSetting);
        CHttpPostDownload();
        ~CHttpPostDownload();

    public:
        void setProgressDelegate(FileProgressDelegate progress);
        virtual void setCancelDelegate(LibCurl::CancelDelegate cancel);

        void setBinaryMode(bool isBinaryMode);
        void setPostData(std::string &postData);
        void setFileBufferSize(UInt32 bufferSize);

		ErrorCode downloadFile(const char* url, const std::string &file, UInt32 lowSpeedTimeoutSec = 5 * 60);
        
    private:
        virtual void _curlSetRecvDataFunction();
        virtual void _curlSetDIY();
        bool _cancelDelegate();
    };
}

//--------------------------------------------------------------------------
#endif /* __LIBCURLHTTPPOSTDOWNLOAD_H__ */