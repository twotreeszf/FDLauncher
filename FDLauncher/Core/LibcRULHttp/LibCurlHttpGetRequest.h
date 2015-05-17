/* -------------------------------------------------------------------------
//	File Name	:	LibCurlHttpGetRequest.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-20 9:42:09
//	Description	:
//
// -----------------------------------------------------------------------*/

#ifndef __LIBCURLHTTPGET_H__
#define __LIBCURLHTTPGET_H__

#include "../Misc/CrossPlatformConfig.h"
#include "LibCurlHttpBase.h"

// -------------------------------------------------------------------------

namespace LibCurl
{
    class CHttpGetRequest
        : public CHttpBase
    {
    public:
        explicit CHttpGetRequest(const TransferSetting &transferSettings);
        CHttpGetRequest();
        ~CHttpGetRequest();

    public:
        ErrorCode requestURL(const char* url, UInt32 timeoutSec);

    private:
        virtual void _curlSetDIY();
    };
}

//--------------------------------------------------------------------------
#endif /* __LIBCURLHTTPGET_H__ */