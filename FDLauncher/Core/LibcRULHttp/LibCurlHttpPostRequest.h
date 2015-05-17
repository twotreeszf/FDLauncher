/* -------------------------------------------------------------------------
//	File Name	:	LibCurlHttpPost.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-20 11:39:46
//	Description	:
//
// -----------------------------------------------------------------------*/

#ifndef __LIBCURLHTTPPOST_H__
#define __LIBCURLHTTPPOST_H__

#include "../Misc/CrossPlatformConfig.h"
#include "../Misc/ErrorCode.h"
#include "LibCurlHttpBase.h"

// -------------------------------------------------------------------------

namespace LibCurl
{
    class CHttpPostRequest
        : public CHttpBase
    {
    private:
        std::string       m_postData;
        bool              m_isBinaryMode;

    public:
        explicit CHttpPostRequest(const TransferSetting &transferSetting);
        CHttpPostRequest();
        ~CHttpPostRequest();

    public:
        void setBinaryMode(bool isBinaryMode);
        void setPostData(std::string &postData);
        ErrorCode requestURL(const char* url, UInt32 timeoutSec);

    private:
        virtual void _curlSetDIY();
    };
}

//--------------------------------------------------------------------------
#endif /* __LIBCURLHTTPPOST_H__ */